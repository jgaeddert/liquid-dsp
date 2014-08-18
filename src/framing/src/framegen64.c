/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

//
// framegen64.c
//
// frame64 generator: 24-byte header, 64-byte payload, 2048-sample frame
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <complex.h>

#include "liquid.internal.h"

// 
// internal method declarations
//

// convert one 8-bit byte to four 2-bit symbols
//  _byte   :   input byte
//  _syms   :   output symbols [size: 4 x 1]
void framegen64_byte_to_syms(unsigned char _byte,
                             unsigned char * _syms);

struct framegen64_s {
    unsigned int m;         // filter delay (symbols)
    float beta;             // filter excess bandwidth factor

    crc_scheme check;       // cyclic redundancy check
    fec_scheme fec0;        // outer forward error-correction code
    fec_scheme fec1;        // inner forward error-correction code
    packetizer p_payload;   // payload packetizer
    modem mod;              // QPSK modulator

    float complex pn_sequence[64];  // 64-symbol p/n sequence

    // payload (QPSK)
    unsigned char payload_dec[72];  // 72 bytes decoded payload data (8 header, 64 payload)
    unsigned char payload_enc[150]; // 150 = (8-byte header, 64-byte payload)=72, crc24, g2412
    unsigned char payload_sym[600]; // 600 = 150 bytes * 8 bits/bytes / 2 bits/symbol

    // pulse-shaping filter
    firinterp_crcf interp;
};

// create framegen64 object
// TODO : permit different p/n sequence?
framegen64 framegen64_create()
{
    framegen64 q = (framegen64) malloc(sizeof(struct framegen64_s));
    q->m    = 3;
    q->beta = 0.5f;

    unsigned int i;

    // generate pn sequence
    msequence ms = msequence_create(6, 0x0043, 1);
    for (i=0; i<64; i++)
        q->pn_sequence[i] = (msequence_advance(ms)) ? 1.0f : -1.0f;
    msequence_destroy(ms);

    // create pulse-shaping filter (k=2)
    q->interp = firinterp_crcf_create_rnyquist(LIQUID_FIRFILT_ARKAISER,2,q->m,q->beta,0);

    // create payload packetizer
    q->check     = LIQUID_CRC_24;
    q->fec0      = LIQUID_FEC_NONE;
    q->fec1      = LIQUID_FEC_GOLAY2412;
    q->p_payload = packetizer_create(72, q->check, q->fec0, q->fec1);
    assert( packetizer_get_enc_msg_len(q->p_payload) == 150 );

    // create modulator
    q->mod = modem_create(LIQUID_MODEM_QPSK);

    return q;
}

// destroy framegen64 object
void framegen64_destroy(framegen64 _q)
{
    // destroy internal objects
    firinterp_crcf_destroy(_q->interp);     // interpolator (matched filter)
    packetizer_destroy(_q->p_payload);      // payload packetizer (encoder)
    modem_destroy(_q->mod);                 // QPSK payload modulator

    // free main object memory
    free(_q);
}

// print framegen64 object internals
void framegen64_print(framegen64 _q)
{
    float eta = (float) (8*(64 + 8)) / (float) 670;
    printf("framegen64 [m=%u, beta=%4.2f]:\n", _q->m, _q->beta);
    printf("  preamble/etc.\n");
    printf("    * ramp/up symbols       :   %u\n", 3);
    printf("    * p/n symbols           :   64\n");
    printf("    * ramp\\down symbols     :   %u\n", 3);
    printf("  payload\n");
    printf("    * payload crc           :   %s\n", crc_scheme_str[_q->check][1]);
    printf("    * fec (inner)           :   %s\n", fec_scheme_str[_q->fec0][1]);
    printf("    * fec (outer)           :   %s\n", fec_scheme_str[_q->fec1][1]);
    printf("    * payload len, coded    :   %u bytes\n", 150);
    printf("    * modulation scheme     :   %s\n", modulation_types[LIQUID_MODEM_QPSK].name);
    printf("    * payload symbols       :   600\n");
    printf("  summary\n");
    printf("    * total symbols         :   670\n");
    printf("    * spectral efficiency   :   %6.4f b/s/Hz\n", eta);
}

// execute frame generator (creates a frame)
//  _q          :   frame generator object
//  _header     :   8-byte header data
//  _payload    :   64-byte payload data
//  _frame      :   output frame samples [size: LIQUID_FRAME64_LEN x 1]
void framegen64_execute(framegen64      _q,
                        unsigned char * _header,
                        unsigned char * _payload,
                        float complex * _frame)
{
    unsigned int i;

    // concatenate header and payload
    memmove(&_q->payload_dec[0], _header,   8*sizeof(unsigned char));
    memmove(&_q->payload_dec[8], _payload, 64*sizeof(unsigned char));

    // encode payload and scramble result
    packetizer_encode(_q->p_payload, _q->payload_dec, _q->payload_enc);
    scramble_data(_q->payload_enc, 150);

    // generate payload symbols
    // 150 bytes -> 600 symbols
    for (i=0; i<150; i++)
        framegen64_byte_to_syms(_q->payload_enc[i], &(_q->payload_sym[4*i]));

    unsigned int n=0;

    // reset interpolator
    firinterp_crcf_reset(_q->interp);

    // p/n sequence
    for (i=0; i<64; i++) {
        firinterp_crcf_execute(_q->interp, _q->pn_sequence[i], &_frame[n]);
        n+=2;
    }

    float complex x;
    // payload
    for (i=0; i<600; i++) {
        modem_modulate(_q->mod, _q->payload_sym[i], &x);
        firinterp_crcf_execute(_q->interp, x, &_frame[n]);
        n+=2;
    }

    // interpolator settling
    for (i=0; i<2*_q->m; i++) {
        firinterp_crcf_execute(_q->interp, 0.0f, &_frame[n]);
        n+=2;
    }

    assert(n==LIQUID_FRAME64_LEN);
}


// 
// Internal
// 

// convert one 8-bit byte to four 2-bit symbols
//  _byte   :   input byte
//  _syms   :   output symbols [size: 4 x 1]
void framegen64_byte_to_syms(unsigned char   _byte,
                             unsigned char * _syms)
{
    _syms[0] = (_byte >> 6) & 0x03;
    _syms[1] = (_byte >> 4) & 0x03;
    _syms[2] = (_byte >> 2) & 0x03;
    _syms[3] = (_byte     ) & 0x03;
}

