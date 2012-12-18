/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2012 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2012 Virginia Polytechnic
 *                                      Institute & State University
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

#define DEBUG_FRAMEGEN64    0

// internal
//void framegen64_encode_header(unsigned char * _header_dec, unsigned char * _header_enc);

struct framegen64_s {
    unsigned int m;         // filter delay (symbols)
    float beta;             // filter excess bandwidth factor

    modem mod;              // QPSK modulator
    packetizer p_payload;   // payload packetizer

    float complex pn_sequence[64];  // 64-symbol p/n sequence

    // payload (QPSK)
    unsigned char payload_dec[64];  // 64 bytes decoded payload data
    unsigned char payload_enc[138]; // 138 = 64 bytes, crc16, g2412
    unsigned char payload_sym[552]; // 552 = 138 bytes * 8 bits/bytes / 2 bits/symbol

    // pulse-shaping filter
    interp_crcf interp;
};

// create framegen64 object
// TODO : remove options for filter parameters (create takes no arguments)
// TODO : permit different p/n sequence?
//  _m      :   rrcos filter delay (number of symbols)
//  _beta   :   rrcos filter excess bandwidth factor
framegen64 framegen64_create(unsigned int _m,
                             float        _beta)
{
    // validate input
    if (_m == 0) {
        fprintf(stderr,"error: framegen64_create(), m must be greater than zero\n");
        exit(1);
    } else if (_beta < 0.0f || _beta > 1.0f) {
        fprintf(stderr,"error: framegen64_create(), beta must be in [0,1]\n");
        exit(1);
    }

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
    q->interp = interp_crcf_create_rnyquist(LIQUID_RNYQUIST_ARKAISER,2,q->m,q->beta,0);

    // create payload packetizers
    unsigned int n      = 64;
    crc_scheme check    = LIQUID_CRC_16;
    fec_scheme fec0     = LIQUID_FEC_NONE;
    fec_scheme fec1     = LIQUID_FEC_GOLAY2412;
    q->p_payload = packetizer_create(n, check, fec0, fec1);

    // create modulator
    q->mod = modem_create(LIQUID_MODEM_QPSK);

    return q;
}

// destroy framegen64 object
void framegen64_destroy(framegen64 _q)
{
    // destroy internal objects
    interp_crcf_destroy(_q->interp);       // interpolator (matched filter)
    packetizer_destroy(_q->p_payload);     // payload packetizer (decoder)
    modem_destroy(_q->mod);                // QPSK payload modulator

    // free main object memory
    free(_q);
}

// print framegen64 object internals
void framegen64_print(framegen64 _q)
{
    printf("framegen64 [m=%u, beta=%4.2f]:\n", _q->m, _q->beta);
    printf("    ramp/up symbols     :   %u\n", 3);
    printf("    p/n symbols         :   64\n");
    printf("    payload symbols     :   552\n");
    printf("    ramp\\down symbols   :   %u\n", 3);
    printf("    total symbols       :   622\n");
}

// execute frame generator (creates a frame)
//  _q          :   frame generator object
//  _header     :   12-byte input header
//  _payload    :   64-byte input payload
//  _y          :   1244-sample frame
void framegen64_execute(framegen64      _q,
                        unsigned char * _header,
                        unsigned char * _payload,
                        float complex * _y)
{
    unsigned int i;

    // encode payload and scramble result
    packetizer_encode(_q->p_payload, _payload, _q->payload_enc);
    scramble_data(_q->payload_enc, 138);

    // generate payload symbols
    // 138 symbols -> 552 
    for (i=0; i<138; i++)
        framegen64_byte_to_syms(_q->payload_enc[i], &(_q->payload_sym[4*i]));

    unsigned int n=0;

    // reset interpolator
    interp_crcf_clear(_q->interp);

    // p/n sequence
    for (i=0; i<64; i++) {
        interp_crcf_execute(_q->interp, _q->pn_sequence[i], &_y[n]);
        n+=2;
    }

    float complex x;
    // payload
    for (i=0; i<552; i++) {
        modem_modulate(_q->mod, _q->payload_sym[i], &x);
        interp_crcf_execute(_q->interp, x, &_y[n]);
        n+=2;
    }

    // settling
    for (i=0; i<6; i++) {
        interp_crcf_execute(_q->interp, 0.0f, &_y[n]);
        n+=2;
    }

    assert(n==1244);
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

