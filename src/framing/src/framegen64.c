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

#define FRAME64_RAMP_UP_LEN 64
#define FRAME64_PHASING_LEN 64
#define FRAME64_PN_LEN      64
#define FRAME64_RAMP_DN_LEN 64

#define FRAMEGEN64_PHASING_0    ( 1.0f) //( 0.70711f + 0.70711f*_Complex_I)
#define FRAMEGEN64_PHASING_1    (-1.0f) //(-0.70711f - 0.70711f*_Complex_I)

#define DEBUG_FRAMEGEN64    0

// internal
//void framegen64_encode_header(unsigned char * _header_dec, unsigned char * _header_enc);

struct framegen64_s {
    modem mod;              // QPSK modulator
    packetizer p_header;    // header packetizer
    packetizer p_payload;   // payload packetizer

    // buffers: preamble (BPSK)
    float complex ramp_up[FRAME64_RAMP_UP_LEN];
    float complex phasing[FRAME64_PHASING_LEN];
    float complex pn_sequence[FRAME64_PN_LEN];
    float complex ramp_dn[FRAME64_RAMP_DN_LEN];

    // header (QPSK)
    unsigned char header_enc[56];   // 56 = (24+4)*2
    unsigned char header_sym[224];  // 224 = 56*4

    // payload (QPSK)
    unsigned char payload_enc[136]; // 136 = (64+4)*2
    unsigned char payload_sym[544]; // 544 = 136*2

    // pulse-shaping filter
    interp_crcf interp;
};

// create framegen64 object
//  _m      :   rrcos filter delay (number of symbols)
//  _beta   :   rrcos filter excess bandwidth factor
framegen64 framegen64_create(unsigned int _m,
                             float _beta)
{
    framegen64 fg = (framegen64) malloc(sizeof(struct framegen64_s));

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
    msequence ms = msequence_create(6);
    for (i=0; i<64; i++)
        fg->pn_sequence[i] = (msequence_advance(ms)) ? FRAMEGEN64_PHASING_1 : FRAMEGEN64_PHASING_0;
    msequence_destroy(ms);

    // create pulse-shaping filter (k=2)
    unsigned int h_len = 2*2*_m + 1;
    float h[h_len];
    design_rrc_filter(2,_m,_beta,0,h);
    fg->interp = interp_crcf_create(2, h, h_len);

    // create header/payload packetizers
    fg->p_header  = packetizer_create(24, FEC_NONE, FEC_HAMMING74);
    fg->p_payload = packetizer_create(64, FEC_NONE, FEC_HAMMING74);

    // create modulator
    fg->mod = modem_create(MOD_QPSK, 2);

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
    printf("framegen:\n");
}

// execute frame generator (creates a frame)
//  _fg         :   frame generator object
//  _header     :   24-byte input header
//  _payload    :   64-byte input payload
//  _y          :   2048-sample frame
void framegen64_execute(framegen64 _fg,
                        unsigned char * _header,
                        unsigned char * _payload,
                        float complex * _y)
{
    unsigned int i;

    // encode header and scramble result
    packetizer_encode(_fg->p_header, _header, _fg->header_enc);
    scramble_data(_fg->header_enc, 56);

    // encode payload and scramble result
    packetizer_encode(_fg->p_payload, _payload, _fg->payload_enc);
    scramble_data(_fg->payload_enc, 136);

    // generate header symbols
    for (i=0; i<56; i++)
        framegen64_byte_to_syms(_fg->header_enc[i], &(_fg->header_sym[4*i]));

    // generate payload symbols
    for (i=0; i<136; i++)
        framegen64_byte_to_syms(_fg->payload_enc[i], &(_fg->payload_sym[4*i]));

    unsigned int n=0;

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
    for (i=0; i<224; i++) {
        modem_modulate(_fg->mod, _fg->header_sym[i], &x);
        interp_crcf_execute(_fg->interp, x, &_y[n]);
        n+=2;
    }

    // payload
    for (i=0; i<544; i++) {
        modem_modulate(_fg->mod, _fg->payload_sym[i], &x);
        interp_crcf_execute(_fg->interp, x, &_y[n]);
        n+=2;
    }

    // ramp down
    for (i=0; i<FRAME64_RAMP_DN_LEN; i++) {
        interp_crcf_execute(_fg->interp, _fg->ramp_dn[i], &_y[n]);
        n+=2;
    }

    assert(n==2048);
}

// flush frame generator buffer by pushing samples through
// internal interpolator
//  _fg     :   frame generator object
//  _n      :   number of output samples (must be even)
//  _y      :   output sample array pointer [size: _n x 1]
void framegen64_flush(framegen64 _fg,
                      unsigned int _n,
                      float complex * _y)
{
    // validate input
    if (_n % 2) {
        fprintf(stderr,"error: framegen64_flush(), _n must be even\n");
        exit(1);
    }

    unsigned int i;
    for (i=0; i<_n; i+=2)
        interp_crcf_execute(_fg->interp, 0, &_y[i]);
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

