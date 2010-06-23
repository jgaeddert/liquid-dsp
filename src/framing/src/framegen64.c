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

#include "liquid.h"

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
    modem mod;          // QPSK modulator
    fec enc;            // encoder
    interleaver intlv;  // interleaver

    // buffers: preamble (BPSK)
    float complex ramp_up[FRAME64_RAMP_UP_LEN];
    float complex phasing[FRAME64_PHASING_LEN];
    float complex pn_sequence[FRAME64_PN_LEN];
    float complex ramp_dn[FRAME64_RAMP_DN_LEN];

    // header (QPSK)
    unsigned char header[32];
    unsigned char header_enc[64];
    unsigned char header_sym[256];

    // payload (QPSK)
    unsigned char payload[64];
    unsigned char payload_enc[128];
    unsigned char payload_intlv[128];
    unsigned char payload_sym[512];

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

    // create FEC encoder
    fg->enc = fec_create(FEC_HAMMING74, NULL);

    // create interleaver
    fg->intlv = interleaver_create(128, LIQUID_INTERLEAVER_BLOCK);

    // create modulator
    fg->mod = modem_create(MOD_QPSK, 2);

    return fg;
}

// destroy framegen64 object
void framegen64_destroy(framegen64 _fg)
{
    interp_crcf_destroy(_fg->interp);
    fec_destroy(_fg->enc);
    interleaver_destroy(_fg->intlv);
    modem_destroy(_fg->mod);
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

    // copy input header to internal array
    memcpy(_fg->header, _header, 24);

    // copy input payload to internal array
    memcpy(_fg->payload, _payload, 64);

    // compute crc32 on payload, append to header at [24:27]
    unsigned int payload_key = crc32_generate_key(_fg->payload, 64);
    //printf("tx: payload_key: 0x%8x\n", payload_key);
    _fg->header[24] = (payload_key >> 24) & 0xff;
    _fg->header[25] = (payload_key >> 16) & 0xff;
    _fg->header[26] = (payload_key >>  8) & 0xff;
    _fg->header[27] = (payload_key      ) & 0xff;
    //printf("tx: payload_key: 0x%8x\n", payload_key);

    // scramble payload data
    scramble_data(_fg->payload, 64);

    // encode payload
    fec_encode(_fg->enc, 64, _fg->payload, _fg->payload_enc);

    // interleave payload
    interleaver_encode(_fg->intlv, _fg->payload_enc, _fg->payload_intlv);

    // compute crc32 on header, append to header at [28:31]
    unsigned int header_key = crc32_generate_key(_fg->header, 28);
    //printf("tx: header_key:  0x%8x\n", header_key);
    _fg->header[28] = (header_key >> 24) & 0xff;
    _fg->header[29] = (header_key >> 16) & 0xff;
    _fg->header[30] = (header_key >>  8) & 0xff;
    _fg->header[31] = (header_key      ) & 0xff;
    //printf("tx: header_key:  0x%8x\n", header_key);

    // scramble header data
    scramble_data(_fg->header, 32);

    // encode header
    fec_encode(_fg->enc, 32, _fg->header, _fg->header_enc);

    // generate header symbols
    for (i=0; i<64; i++)
        framegen64_byte_to_syms(_fg->header_enc[i], &(_fg->header_sym[4*i]));

    // generate payload symbols
    for (i=0; i<128; i++)
        framegen64_byte_to_syms(_fg->payload_intlv[i], &(_fg->payload_sym[4*i]));

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
    for (i=0; i<256; i++) {
        modem_modulate(_fg->mod, _fg->header_sym[i], &x);
        interp_crcf_execute(_fg->interp, x, &_y[n]);
        n+=2;
    }

    // payload
    for (i=0; i<512; i++) {
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

