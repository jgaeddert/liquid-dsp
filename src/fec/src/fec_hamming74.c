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
// 1/2-rate (7,4) Hamming code
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

// generator polynomials
#define HAMMING74_H0    0x55
#define HAMMING74_H1    0x33
#define HAMMING74_H2    0x0f

// decode Hamming(7,4) symbol by simply stripping out
// original data bits
#define fec_hamming74_decode_symbol(_s) \
    (((0x10 & _s) >> 1) | ((0x07 & _s) >> 0))

// Hamming(7,4) encoding matrix
static unsigned char hamming74_enc[16] = {
    0x00,   0x69,   0x2a,   0x43,
    0x4c,   0x25,   0x66,   0x0f,
    0x70,   0x19,   0x5a,   0x33,
    0x3c,   0x55,   0x16,   0x7f
};

// Hamming(7,4) syndrome bit-flip array
static unsigned char hamming74_bflip[] = {
    0x00,   // 0 (not used)
    0x40,   // 1
    0x20,   // 2
    0x10,   // 3
    0x08,   // 4
    0x04,   // 5
    0x02,   // 6
    0x01,   // 7
};

// create Hamming(7,4) codec object
fec fec_hamming74_create(void * _opts)
{
    fec q = (fec) malloc(sizeof(struct fec_s));

    // set scheme
    q->scheme = FEC_HAMMING74;
    q->rate = fec_get_rate(q->scheme);

    // set internal function pointers
    q->encode_func = &fec_hamming74_encode;
    q->decode_func = &fec_hamming74_decode;

    return q;
}

// destroy Hamming(7,4) object
void fec_hamming74_destroy(fec _q)
{
    free(_q);
}

// encode block of data using Hamming(7,4) encoder
//
//  _q              :   encoder/decoder object
//  _dec_msg_len    :   decoded message length (number of bytes)
//  _msg_dec        :   decoded message [size: 1 x _dec_msg_len]
//  _msg_enc        :   encoded message [size: 1 x 2*_dec_msg_len]
void fec_hamming74_encode(fec _q,
                          unsigned int _dec_msg_len,
                          unsigned char *_msg_dec,
                          unsigned char *_msg_enc)
{
    unsigned int i, j=0;
    unsigned char s0, s1;
    for (i=0; i<_dec_msg_len; i++) {
        s0 = (_msg_dec[i] >> 4) & 0x0f;
        s1 = (_msg_dec[i] >> 0) & 0x0f;
        _msg_enc[j+0] = hamming74_enc[s0];
        _msg_enc[j+1] = hamming74_enc[s1];
        j+=2;
    }
}

// decode block of data using Hamming(7,4) decoder
//
//  _q              :   encoder/decoder object
//  _dec_msg_len    :   decoded message length (number of bytes)
//  _msg_enc        :   encoded message [size: 1 x 2*_dec_msg_len]
//  _msg_dec        :   decoded message [size: 1 x _dec_msg_len]
//
//unsigned int
void fec_hamming74_decode(fec _q,
                          unsigned int _dec_msg_len,
                          unsigned char *_msg_enc,
                          unsigned char *_msg_dec)
{
    unsigned int i, num_errors=0;
    unsigned char r0, r1, z0, z1, s0, s1;
    for (i=0; i<_dec_msg_len; i++) {
        r0 = _msg_enc[2*i+0];
        r1 = _msg_enc[2*i+1];

        //printf("%u :\n", i);

        // compute syndromes
        z0 = fec_hamming74_compute_syndrome(r0);
        z1 = fec_hamming74_compute_syndrome(r1);

        //printf("  syndrome[%u]          : %d, %d\n", i, (int)z0, (int)z1);
        //printf("  input symbols[%u]     : 0x%.2x, 0x%.2x\n", i, r0, r1);

        if (z0) r0 ^= hamming74_bflip[z0];
        if (z1) r1 ^= hamming74_bflip[z1];

        num_errors += (z0) ? 1 : 0;
        num_errors += (z1) ? 1 : 0;

        //printf("  corrected symbols[%u] : 0x%.2x, 0x%.2x\n", i, r0, r1);

        s0 = fec_hamming74_decode_symbol(r0);
        s1 = fec_hamming74_decode_symbol(r1);

        //printf("  decoded symbols[%u]   : 0x%.1x%.1x\n", i, s0, s1);

        _msg_dec[i] = (s0 << 4) | s1;
    }
    //return num_errors;
}


// 
// internal
//

// binary dot-product (count ones modulo 2)
#define bdotprod(x,y) c_ones_mod2[(x)&(y)]

// compute syndrome on received symbol _r
unsigned char fec_hamming74_compute_syndrome(unsigned char _r)
{
    return (bdotprod(_r,HAMMING74_H0) << 0) |
           (bdotprod(_r,HAMMING74_H1) << 1) |
           (bdotprod(_r,HAMMING74_H2) << 2);
}

