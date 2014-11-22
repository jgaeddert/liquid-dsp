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
// interleaver_create.c
//
// Create and initialize interleaver objects
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "liquid.internal.h"

// 
// internal methods
//

// permute one iteration
void interleaver_permute(unsigned char * _x,
                         unsigned int    _n,
                         unsigned int    _M,
                         unsigned int    _N);

// permute one iteration (soft bit input)
void interleaver_permute_soft(unsigned char * _x,
                              unsigned int    _n,
                              unsigned int    _M,
                              unsigned int    _N);

// permute one iteration with mask
void interleaver_permute_mask(unsigned char * _x,
                              unsigned int    _n,
                              unsigned int    _M,
                              unsigned int    _N,
                              unsigned char   _mask);

// permute one iteration (soft bit input) with mask
void interleaver_permute_mask_soft(unsigned char * _x,
                                   unsigned int    _n,
                                   unsigned int    _M,
                                   unsigned int    _N,
                                   unsigned char   _mask);


// structured interleaver object
struct interleaver_s {
    unsigned int n;     // number of bytes

    unsigned int M;     // row dimension
    unsigned int N;     // col dimension

    // interleaving depth (number of permutations)
    unsigned int depth;
};

// create interleaver of length _n input/output bytes
interleaver interleaver_create(unsigned int _n)
{
    interleaver q = (interleaver) malloc(sizeof(struct interleaver_s));
    q->n = _n;

    // set internal properties
    q->depth = 4;   // default depth to maximum 

    // compute block dimensions
    q->M = 1 + (unsigned int) floorf(sqrtf(q->n));

    q->N = q->n / q->M;
    while (q->n >= (q->M*q->N)) q->N++;  // ensures M*N >= n

    return q;
}

// destroy interleaver object
void interleaver_destroy(interleaver _q)
{
    // free main object memory
    free(_q);
}

// print interleaver internals
void interleaver_print(interleaver _q)
{
    printf("interleaver [block, %u bytes] :\n", _q->n);
    printf("    M       :   %u\n", _q->M);
    printf("    N       :   %u\n", _q->N);
    printf("    depth   :   %u\n", _q->depth);
}

// set depth (number of internal iterations)
void interleaver_set_depth(interleaver  _q,
                           unsigned int _depth)
{
    _q->depth = _depth;
}

// execute forward interleaver (encoder)
//  _q          :   interleaver object
//  _msg_dec    :   decoded (un-interleaved) message
//  _msg_enc    :   encoded (interleaved) message
void interleaver_encode(interleaver _q,
                        unsigned char * _msg_dec,
                        unsigned char * _msg_enc)
{
    // copy data to output
    memmove(_msg_enc, _msg_dec, _q->n);

    if (_q->depth > 0) interleaver_permute(_msg_enc, _q->n, _q->M, _q->N);
    if (_q->depth > 1) interleaver_permute_mask(_msg_enc, _q->n, _q->M, _q->N+2, 0x0f);
    if (_q->depth > 2) interleaver_permute_mask(_msg_enc, _q->n, _q->M, _q->N+4, 0x55);
    if (_q->depth > 3) interleaver_permute_mask(_msg_enc, _q->n, _q->M, _q->N+8, 0x33);
}

// execute forward interleaver (encoder) on soft bits
//  _q          :   interleaver object
//  _msg_dec    :   decoded (un-interleaved) message
//  _msg_enc    :   encoded (interleaved) message
void interleaver_encode_soft(interleaver _q,
                             unsigned char * _msg_dec,
                             unsigned char * _msg_enc)
{
    // copy data to output
    memmove(_msg_enc, _msg_dec, 8*_q->n);

    if (_q->depth > 0) interleaver_permute_soft(_msg_enc, _q->n, _q->M, _q->N);
    if (_q->depth > 1) interleaver_permute_mask_soft(_msg_enc, _q->n, _q->M, _q->N+2, 0x0f);
    if (_q->depth > 2) interleaver_permute_mask_soft(_msg_enc, _q->n, _q->M, _q->N+4, 0x55);
    if (_q->depth > 3) interleaver_permute_mask_soft(_msg_enc, _q->n, _q->M, _q->N+8, 0x33);
}

// execute reverse interleaver (decoder)
//  _q          :   interleaver object
//  _msg_enc    :   encoded (interleaved) message
//  _msg_dec    :   decoded (un-interleaved) message
void interleaver_decode(interleaver _q,
                        unsigned char * _msg_enc,
                        unsigned char * _msg_dec)
{
    // copy data to output
    memmove(_msg_dec, _msg_enc, _q->n);

    if (_q->depth > 3) interleaver_permute_mask(_msg_dec, _q->n, _q->M, _q->N+8, 0x33);
    if (_q->depth > 2) interleaver_permute_mask(_msg_dec, _q->n, _q->M, _q->N+4, 0x55);
    if (_q->depth > 1) interleaver_permute_mask(_msg_dec, _q->n, _q->M, _q->N+2, 0x0f);
    if (_q->depth > 0) interleaver_permute(_msg_dec, _q->n, _q->M, _q->N);
}

// execute reverse interleaver (decoder) on soft bits
//  _q          :   interleaver object
//  _msg_enc    :   encoded (interleaved) message
//  _msg_dec    :   decoded (un-interleaved) message
void interleaver_decode_soft(interleaver _q,
                             unsigned char * _msg_enc,
                             unsigned char * _msg_dec)
{
    // copy data to output
    memmove(_msg_dec, _msg_enc, 8*_q->n);

    if (_q->depth > 3) interleaver_permute_mask_soft(_msg_dec, _q->n, _q->M, _q->N+8, 0x33);
    if (_q->depth > 2) interleaver_permute_mask_soft(_msg_dec, _q->n, _q->M, _q->N+4, 0x55);
    if (_q->depth > 1) interleaver_permute_mask_soft(_msg_dec, _q->n, _q->M, _q->N+2, 0x0f);
    if (_q->depth > 0) interleaver_permute_soft(_msg_dec, _q->n, _q->M, _q->N);
}

// 
// internal permutation methods
//

// permute one iteration
void interleaver_permute(unsigned char * _x,
                         unsigned int    _n,
                         unsigned int    _M,
                         unsigned int    _N)
{
    unsigned int i;
    unsigned int j;
    unsigned int m=0;
    unsigned int n=_n/3;
    unsigned int n2=_n/2;
    unsigned char tmp;
    for (i=0; i<n2; i++) {
        //j = m*N + n; // input
        do {
            j = m*_N + n; // output
            m++;
            if (m == _M) {
                n = (n+1) % (_N);
                m=0;
            }
        } while (j>=n2);

        // swap indices
        tmp = _x[2*j+1];
        _x[2*j+1] = _x[2*i+0];
        _x[2*i+0] = tmp;
    }
}

// permute one iteration (soft bit input)
void interleaver_permute_soft(unsigned char * _x,
                              unsigned int    _n,
                              unsigned int    _M,
                              unsigned int    _N)
{
    unsigned int i;
    unsigned int j;
    unsigned int m=0;
    unsigned int n=_n/3;
    unsigned int n2=_n/2;
    unsigned char tmp[8];
    for (i=0; i<n2; i++) {
        //j = m*N + n; // input
        do {
            j = m*_N + n; // output
            m++;
            if (m == _M) {
                n = (n+1) % (_N);
                m=0;
            }
        } while (j>=n2);
    
        // swap soft bits at indices
        memmove( tmp,            &_x[8*(2*j+1)], 8);
        memmove( &_x[8*(2*j+1)], &_x[8*(2*i+0)], 8);
        memmove( &_x[8*(2*i+0)], tmp,            8);
    }
    //printf("\n");
}


// permute one iteration with mask
void interleaver_permute_mask(unsigned char * _x,
                              unsigned int    _n,
                              unsigned int    _M,
                              unsigned int    _N,
                              unsigned char   _mask)
{
    unsigned int i;
    unsigned int j;
    unsigned int m=0;
    unsigned int n=_n/3;
    unsigned int n2=_n/2;
    unsigned char tmp0;
    unsigned char tmp1;
    for (i=0; i<n2; i++) {
        //j = m*N + n; // input
        do {
            j = m*_N + n; // output
            m++;
            if (m == _M) {
                n = (n+1) % (_N);
                m=0;
            }
        } while (j>=n2);

        // swap indices, applying mask
        tmp0 = (_x[2*i+0] & (~_mask)) | (_x[2*j+1] & ( _mask));
        tmp1 = (_x[2*i+0] & ( _mask)) | (_x[2*j+1] & (~_mask));
        _x[2*i+0] = tmp0;
        _x[2*j+1] = tmp1;
    }
}

// permute one iteration (soft bit input)
void interleaver_permute_mask_soft(unsigned char * _x,
                                   unsigned int    _n,
                                   unsigned int    _M,
                                   unsigned int    _N,
                                   unsigned char   _mask)
{
    unsigned int i;
    unsigned int j;
    unsigned int k;
    unsigned int m=0;
    unsigned int n=_n/3;
    unsigned int n2=_n/2;
    unsigned char tmp;
    for (i=0; i<n2; i++) {
        //j = m*N + n; // input
        do {
            j = m*_N + n; // output
            m++;
            if (m == _M) {
                n = (n+1) % (_N);
                m=0;
            }
        } while (j>=n2);

        // swap bits matching the mask
        for (k=0; k<8; k++) {
            if ( (_mask >> (8-k-1)) & 0x01 ) {
                tmp = _x[8*(2*j+1)+k];
                _x[8*(2*j+1)+k] = _x[8*(2*i+0)+k];
                _x[8*(2*i+0)+k] = tmp;
            }
        }
    }
    //printf("\n");
}

