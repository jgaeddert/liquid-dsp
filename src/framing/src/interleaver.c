/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Virginia Polytechnic
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
// interleaver_create.c
//
// Create and initialize interleaver objects
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "liquid.internal.h"

interleaver interleaver_create(unsigned int _n,
                               interleaver_type _type)
{
    interleaver q = (interleaver) malloc(sizeof(struct interleaver_s));
    q->n = _n;
    q->num_iterations = 1;

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
    printf("interleaver [%u] :\n", _q->n);
    printf("    M   :   %u\n", _q->M);
    printf("    N   :   %u\n", _q->N);
}

// execute forward interleaver (encoder)
//  _q          :   interleaver object
//  _msg_dec    :   decoded (un-interleaved) message
//  _msg_enc    :   encoded (interleaved) message
void interleaver_encode(interleaver _q,
                        unsigned char * _msg_dec,
                        unsigned char * _msg_enc)
{
    // single iteration
    memmove(_msg_enc, _msg_dec, _q->n);
    interleaver_permute(_msg_enc, _q->n, _q->M, _q->N);
}

// execute forward interleaver (encoder) on soft bits
//  _q          :   interleaver object
//  _msg_dec    :   decoded (un-interleaved) message
//  _msg_enc    :   encoded (interleaved) message
void interleaver_encode_soft(interleaver _q,
                             unsigned char * _msg_dec,
                             unsigned char * _msg_enc)
{
    // single iteration
    memmove(_msg_enc, _msg_dec, 8*_q->n);
    interleaver_permute_soft(_msg_enc, _q->n, _q->M, _q->N);
}

// execute reverse interleaver (decoder)
//  _q          :   interleaver object
//  _msg_enc    :   encoded (interleaved) message
//  _msg_dec    :   decoded (un-interleaved) message
void interleaver_decode(interleaver _q,
                        unsigned char * _msg_enc,
                        unsigned char * _msg_dec)
{
    // single iteration
    memmove(_msg_dec, _msg_enc, _q->n);
    interleaver_permute(_msg_dec, _q->n, _q->M, _q->N);
}

// execute reverse interleaver (decoder) on soft bits
//  _q          :   interleaver object
//  _msg_enc    :   encoded (interleaved) message
//  _msg_dec    :   decoded (un-interleaved) message
void interleaver_decode_soft(interleaver _q,
                             unsigned char * _msg_enc,
                             unsigned char * _msg_dec)
{
    // single iteration
    memmove(_msg_dec, _msg_enc, 8*_q->n);
    interleaver_permute_soft(_msg_dec, _q->n, _q->M, _q->N);
}

// set number of internal iterations
void interleaver_set_num_iterations(interleaver _q,
                                    unsigned int _n)
{
}


// permute one iteration
void interleaver_permute(unsigned char * _x,
                         unsigned int _n,
                         unsigned int _M,
                         unsigned int _N)
{
    unsigned int i;
    unsigned int j;
    unsigned int m=0;
    unsigned int n=0;
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
                              unsigned int _n,
                              unsigned int _M,
                              unsigned int _N)
{
    unsigned int i;
    unsigned int j;
    unsigned int m=0;
    unsigned int n=0;
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


// permute forward one iteration with byte mask
//  _x      :   input/output data array, [size: _n x 1]
//  _n      :   array size
//  _mask   :   byte mask
void interleaver_permute_forward_mask(unsigned char * _x,
                                      unsigned int _n,
                                      unsigned char _mask)
{
}

// permute reverse one iteration with byte mask
//  _x      :   input/output data array, [size: _n x 1]
//  _n      :   array size
//  _mask   :   byte mask
void interleaver_permute_reverse_mask(unsigned char * _x,
                                      unsigned int _n,
                                      unsigned char _mask)
{
}


// compute bit permutation for interleaver
//  _q      :   interleaver object
//  _p      :   output permutation index array, [size: 8*_n x 1]
void interleaver_compute_bit_permutation(interleaver _q,
                                         unsigned int * _p)
{
    unsigned int i, j;
    unsigned char x[_q->n], y[_q->n];

    for (i=0; i<_q->n; i++)
        x[i] = 0;

    for (i=0; i<_q->n; i++) {
        for (j=0; j<8; j++) {
            x[i] = 1<<j;
            interleaver_encode(_q, x, y);
            // find where the bit went!
            // look for byte containing bit
            unsigned int k;
            for (k=0; k<_q->n; k++) {
                if (y[k] > 0)
                    break;
            }
            // find bit position
            unsigned char v = y[k];
            unsigned int r;
            for (r=0; r<8; r++) {
                if (v & 1)
                    break;
                v >>= 1;
            }
            _p[8*i + j] = 8*k + r;
        }
        x[i] = 0;
    }
}

// print interleaver internals with debugging info
void interleaver_debug_print(interleaver _q)
{
    interleaver_print(_q);
    return;
    unsigned int n = (_q->n)*sizeof(unsigned char)*8;
    if (n>80) {
        printf("interleaver_debug_print(), too large to print debug info\n");
        return;
    }

    unsigned int t[n];
    interleaver_compute_bit_permutation(_q, t);

    unsigned int i,j,k;

    // compute permutation metrics: distance between bits
    float dtmp, dki, dmin=(float)n, dmean=0.0f;
    j = 0;
    for (k=1; k<3; k++) {
        //printf("==== k : %d\n", k);
        for (i=0; i<(n-k); i++) {
            dki = fabsf((float)(t[i]) - (float)(t[i+k]));
            dtmp = dki + (float)(k-1);
            //printf("    d(%u,%u) : %f\n", i, i+k, dki);

            dmean += dki;
            dmin = (dtmp < dmin) ? dki : dmin;
            j++;
        }
    }
    dmean /= j;

    printf("   ");
    j=0;
    for (i=0; i<n; i++) {
        if ((i%10)==0)  printf("%1u", j++);
        else            printf(" ");
    }
    printf("\n");

    for (i=0; i<n; i++) {
        printf("%2u ", i);
        for (j=0; j<n; j++) {
            if (j==t[i])
                printf("*");
            else if ((j%10)==0 && ((i%10)==0))
                printf("+");
            else if ((j%10)==0)
                printf("|");
            else if ((i%10)==0)
                printf("-");
            else if (j==i)
                printf("\\");
            else
                printf(" ");
        }
        printf("\n");
    }
    printf("\n");
    printf("  dmin: %8.2f, dmean: %8.2f\n", dmin, dmean);
}

