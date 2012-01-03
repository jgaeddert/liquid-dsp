/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
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

#define FRAME64_RAMP_UP_LEN 12
#define FRAME64_PHASING_LEN 64
#define FRAME64_PN_LEN      64
#define FRAME64_RAMP_DN_LEN 12
#define FRAME64_SETTLE_LEN  8

#define FRAMEGEN64_PHASING_0    ( 1.0f) //( 0.70711f + 0.70711f*_Complex_I)
#define FRAMEGEN64_PHASING_1    (-1.0f) //(-0.70711f - 0.70711f*_Complex_I)

#define DEBUG_FRAMEGEN64    0

// internal
//void framegen64_encode_header(unsigned char * _header_dec, unsigned char * _header_enc);

struct framegen64_s {
    unsigned int m;         // filter delay (symbols)
    float beta;             // filter excess bandwidth factor

    modem mod;              // QPSK modulator
    packetizer p_header;    // header packetizer
    packetizer p_payload;   // payload packetizer

    // buffers: preamble (BPSK)
    float complex ramp_up[FRAME64_RAMP_UP_LEN];
    float complex phasing[FRAME64_PHASING_LEN];
    float complex pn_sequence[FRAME64_PN_LEN];
    float complex ramp_dn[FRAME64_RAMP_DN_LEN];

    // header (QPSK)
    unsigned char header_enc[21];   // 21 = 12 bytes, crc16, h128
    unsigned char header_sym[84];   // 84 = 21*4

    // payload (QPSK)
    unsigned char payload_enc[99];  // 99 = 64 bytes, crc16, h128
    unsigned char payload_sym[396]; // 396 = 99*4

    // pulse-shaping filter
    interp_crcf interp;
};

// create framegen64 object
//  _m      :   rrcos filter delay (number of symbols)
//  _beta   :   rrcos filter excess bandwidth factor
framegen64 framegen64_create(unsigned int _m,
                             float _beta)
{
    // validate input
    if (_m == 0) {
        fprintf(stderr,"error: framegen64_create(), m must be greater than zero\n");
        exit(1);
    } else if (_beta < 0.0f || _beta > 1.0f) {
        fprintf(stderr,"error: framegen64_create(), beta must be in [0,1]\n");
        exit(1);
    }

    framegen64 fg = (framegen64) malloc(sizeof(struct framegen64_s));
    fg->m    = _m;
    fg->beta = _beta;

    unsigned int i;

    // generate ramp_up
    for (i=0; i<FRAME64_RAMP_UP_LEN; i++) {
        fg->ramp_up[i] = (i%2) ? FRAMEGEN64_PHASING_1 : FRAMEGEN64_PHASING_0;
        //fg->ramp_up[i] *= kaiser(i,2*FRAME64_RAMP_UP_LEN,8.0f,0.0f);
        //fg->ramp_up[i] *= (float)(i) / (float)(FRAME64_RAMP_UP_LEN);
        fg->ramp_up[i] *= 0.5f*(1.0f - cosf(M_PI * (float)(i) / (float)(FRAME64_RAMP_UP_LEN)));
    }

    // generate ramp_dn
    for (i=0; i<FRAME64_RAMP_DN_LEN; i++) {
        fg->ramp_dn[i] = (i%2) ? FRAMEGEN64_PHASING_1 : FRAMEGEN64_PHASING_0;
        //fg->ramp_dn[i] *= kaiser(i+FRAME64_RAMP_DN_LEN,2*FRAME64_RAMP_DN_LEN,8.0f,0.0f);
        //fg->ramp_dn[i] *= (float)(FRAME64_RAMP_DN_LEN-i-1) / (float)(FRAME64_RAMP_DN_LEN);
        fg->ramp_dn[i] *= 0.5f*(1.0f + cos(M_PI * (float)(i) / (float)(FRAME64_RAMP_DN_LEN)));
    }

    // generate phasing pattern
    for (i=0; i<FRAME64_PHASING_LEN; i++)
        fg->phasing[i] = (i%2) ? FRAMEGEN64_PHASING_1 : FRAMEGEN64_PHASING_0;

    // generate pn sequence
    msequence ms = msequence_create(6, 0x0043, 1);
    for (i=0; i<64; i++)
        fg->pn_sequence[i] = (msequence_advance(ms)) ? FRAMEGEN64_PHASING_1 : FRAMEGEN64_PHASING_0;
    msequence_destroy(ms);

    // create pulse-shaping filter (k=2)
    fg->interp = interp_crcf_create_rnyquist(LIQUID_RNYQUIST_ARKAISER,2,fg->m,fg->beta,0);

    // create header/payload packetizers
    fg->p_header  = packetizer_create(12, LIQUID_CRC_16, LIQUID_FEC_NONE, LIQUID_FEC_HAMMING128);
    fg->p_payload = packetizer_create(64, LIQUID_CRC_16, LIQUID_FEC_NONE, LIQUID_FEC_HAMMING128);

    // create modulator
    fg->mod = modem_create(LIQUID_MODEM_QPSK);

    return fg;
}

// destroy framegen64 object
void framegen64_destroy(framegen64 _fg)
{
    interp_crcf_destroy(_fg->interp);       // interpolator (matched filter)
    packetizer_destroy(_fg->p_header);      // header packetizer (encoder)
    packetizer_destroy(_fg->p_payload);     // payload packetizer (decoder)
    modem_destroy(_fg->mod);                // QPSK header/payload modulator

    // free main object memory
    free(_fg);
}

// print framegen64 object internals
void framegen64_print(framegen64 _fg)
{
    printf("framegen64 [m=%u, beta=%4.2f]:\n", _fg->m, _fg->beta);
    printf("    ramp/up symbols     :   %u\n", FRAME64_RAMP_UP_LEN);
    printf("    phasing symbols     :   64\n");
    printf("    p/n symbols         :   64\n");
    printf("    header symbols      :   84\n");
    printf("    payload symbols     :   396\n");
    printf("    payload symbols     :   396\n");
    printf("    ramp\\down symbols   :   %u\n", FRAME64_RAMP_DN_LEN);
    printf("    settling symbols    :   %u\n", FRAME64_SETTLE_LEN);
    printf("    total symbols       :   640\n");
}

// execute frame generator (creates a frame)
//  _fg         :   frame generator object
//  _header     :   12-byte input header
//  _payload    :   64-byte input payload
//  _y          :   1280-sample frame
void framegen64_execute(framegen64 _fg,
                        unsigned char * _header,
                        unsigned char * _payload,
                        float complex * _y)
{
    unsigned int i;

    // encode header and scramble result
    packetizer_encode(_fg->p_header, _header, _fg->header_enc);
    scramble_data(_fg->header_enc, 21);

    // encode payload and scramble result
    packetizer_encode(_fg->p_payload, _payload, _fg->payload_enc);
    scramble_data(_fg->payload_enc, 99);

    // generate header symbols
    for (i=0; i<21; i++)
        framegen64_byte_to_syms(_fg->header_enc[i], &(_fg->header_sym[4*i]));

    // generate payload symbols
    for (i=0; i<99; i++)
        framegen64_byte_to_syms(_fg->payload_enc[i], &(_fg->payload_sym[4*i]));

    unsigned int n=0;

    // reset interpolator
    interp_crcf_clear(_fg->interp);

    // ramp up
    for (i=0; i<FRAME64_RAMP_UP_LEN; i++) {
        interp_crcf_execute(_fg->interp, _fg->ramp_up[i], &_y[n]);
        n+=2;
    }

    // phasing
    for (i=0; i<FRAME64_PHASING_LEN; i++) {
        interp_crcf_execute(_fg->interp, _fg->phasing[i], &_y[n]);
        n+=2;
    }

    // p/n sequence
    for (i=0; i<FRAME64_PN_LEN; i++) {
        interp_crcf_execute(_fg->interp, _fg->pn_sequence[i], &_y[n]);
        n+=2;
    }

    float complex x;
    // header
    for (i=0; i<84; i++) {
        modem_modulate(_fg->mod, _fg->header_sym[i], &x);
        interp_crcf_execute(_fg->interp, x, &_y[n]);
        n+=2;
    }

    // payload
    for (i=0; i<396; i++) {
        modem_modulate(_fg->mod, _fg->payload_sym[i], &x);
        interp_crcf_execute(_fg->interp, x, &_y[n]);
        n+=2;
    }

    // ramp down
    for (i=0; i<FRAME64_RAMP_DN_LEN; i++) {
        interp_crcf_execute(_fg->interp, _fg->ramp_dn[i], &_y[n]);
        n+=2;
    }

    // settling
    for (i=0; i<FRAME64_SETTLE_LEN; i++) {
        interp_crcf_execute(_fg->interp, 0.0f, &_y[n]);
        n+=2;
    }

    assert(n==1280);
}


// 
// Internal
// 

// convert one 8-bit byte to four 2-bit symbols
//  _byte   :   input byte
//  _syms   :   output symbols [size: 4 x 1]
void framegen64_byte_to_syms(unsigned char _byte,
                             unsigned char * _syms)
{
    _syms[0] = (_byte >> 6) & 0x03;
    _syms[1] = (_byte >> 4) & 0x03;
    _syms[2] = (_byte >> 2) & 0x03;
    _syms[3] = (_byte     ) & 0x03;
}

