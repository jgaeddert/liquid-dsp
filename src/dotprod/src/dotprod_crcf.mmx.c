/*
 * Copyright (c) 2012 Joseph Gaeddert
 * Copyright (c) 2012 Virginia Polytechnic Institute & State University
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
 * MERCHANTABILITY or FITNESS FOR A PARfloatCULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

// 
// Floating-point dot product (MMX)
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xmmintrin.h>
#include <assert.h>

#include "liquid.internal.h"

#define DEBUG_DOTPROD_CRCF_MMX   0

// forward declaration of internal methods
void dotprod_crcf_execute_mmx(dotprod_crcf _q,
                              float complex * _x,
                              float complex * _y);
void dotprod_crcf_execute_mmx4(dotprod_crcf _q,
                               float complex * _x,
                               float complex * _y);

// basic dot product (ordinal calculation)
void dotprod_crcf_run(float *         _h,
                      float complex * _x,
                      unsigned int    _n,
                      float complex * _y)
{
    float complex r = 0;
    unsigned int i;
    for (i=0; i<_n; i++)
        r += _h[i] * _x[i];
    *_y = r;
}

// basic dot product (ordinal calculation) with loop unrolled
void dotprod_crcf_run4(float *         _h,
                       float complex * _x,
                       unsigned int    _n,
                       float complex * _y)
{
    float complex r = 0;

    // t = 4*(floor(_n/4))
    unsigned int t=(_n>>2)<<2; 

    // compute dotprod in groups of 4
    unsigned int i;
    for (i=0; i<t; i+=4) {
        r += _h[i]   * _x[i];
        r += _h[i+1] * _x[i+1];
        r += _h[i+2] * _x[i+2];
        r += _h[i+3] * _x[i+3];
    }

    // clean up remaining
    for ( ; i<_n; i++)
        r += _h[i] * _x[i];

    *_y = r;
}


//
// structured MMX dot product
//

struct dotprod_crcf_s {
    unsigned int n;     // length
    float * h;          // coefficients array
};

dotprod_crcf dotprod_crcf_create(float * _h,
                                 unsigned int _n)
{
    dotprod_crcf q = (dotprod_crcf)malloc(sizeof(struct dotprod_crcf_s));
    q->n = _n;

    // allocate memory for coefficients
    // TODO : check memory alignment?
    q->h = (float*) malloc( 2*q->n*sizeof(float) );

    // set coefficients, repeated
    //  h = { _h[0], _h[0], _h[1], _h[1], ... _h[n-1], _h[n-1]}
    unsigned int i;
    for (i=0; i<q->n; i++) {
        q->h[2*i+0] = _h[i];
        q->h[2*i+1] = _h[i];
    }

    // return object
    return q;
}

// re-create the structured dotprod object
dotprod_crcf dotprod_crcf_recreate(dotprod_crcf _dp,
                                   float * _h,
                                   unsigned int _n)
{
    // completely destroy and re-create dotprod object
    dotprod_crcf_destroy(_dp);
    _dp = dotprod_crcf_create(_h,_n);
    return _dp;
}


void dotprod_crcf_destroy(dotprod_crcf _q)
{
    free(_q->h);
    free(_q);
}

void dotprod_crcf_print(dotprod_crcf _q)
{
    printf("dotprod_crcf:\n");
    unsigned int i;
    // print coefficients to screen, skipping odd entries (due
    // to repeated coefficients)
    for (i=0; i<_q->n; i++)
        printf("%3u : %12.9f\n", i, _q->h[2*i]);
}

// 
void dotprod_crcf_execute(dotprod_crcf _q,
                          float complex * _x,
                          float complex * _y)
{
    // switch based on size
    if (_q->n < 32) {
        dotprod_crcf_execute_mmx(_q, _x, _y);
    } else {
        dotprod_crcf_execute_mmx4(_q, _x, _y);
    }
}

// use MMX/SSE extensions
void dotprod_crcf_execute_mmx(dotprod_crcf _q,
                              float complex * _x,
                              float complex * _y)
{
    // type cast input as floating point array
    float * x = (float*) _x;

    // double effective length
    unsigned int n = 2*_q->n;

#if DEBUG_DOTPROD_CRCF_MMX
    // check alignment
    int align = ((long int)x & 15)/sizeof(float);
    printf("align : %d\n", align);
#endif

    // first cut: ...
    __m128 v;   // input vector
    __m128 h;   // coefficients vector
    __m128 s;   // dot product
    union { __m128 v; float w[4] __attribute__((aligned(16)));} sum;
    sum.v = _mm_set1_ps(0.0f);

    // t = 4*(floor(_n/4))
    unsigned int t = (n >> 2) << 2;

    //
    unsigned int i;
    for (i=0; i<t; i+=4) {
        // load inputs into register (unaligned)
        v = _mm_loadu_ps(&x[i]);

        // load coefficients into register (unaligned)
        // TODO : ensure proper alignment
        h = _mm_loadu_ps(&_q->h[i]);

        // compute multiplication
        s = _mm_mul_ps(v, h);

        // accumulate
        sum.v = _mm_add_ps(sum.v, s);
    }

    // add in-phase and quadrature components
    sum.w[0] += sum.w[2];
    sum.w[1] += sum.w[3];

    // cleanup (note: n _must_ be even)
    for (; i<n; i+=2) {
        sum.w[0] += x[i  ] * _q->h[i  ];
        sum.w[1] += x[i+1] * _q->h[i+1];
    }

    // set return value
    *_y = sum.w[0] + _Complex_I*sum.w[1];
}

// use MMX/SSE extensions
// FIXME : something wrong with this method
void dotprod_crcf_execute_mmx4(dotprod_crcf _q,
                               float complex * _x,
                               float complex * _y)
{
    // type cast input as floating point array
    float * x = (float*) _x;

    // double effective length
    unsigned int n = 2*_q->n;

    // first cut: ...
    __m128 v0, v1, v2, v3;  // input vectors
    __m128 h0, h1, h2, h3;  // coefficients vectors
    __m128 s0, s1, s2, s3;  // dot products [re, im, re, im]

    // load zeros into sum registers
    __m128 sum0 = _mm_set1_ps(0.0f);
    __m128 sum1 = _mm_set1_ps(0.0f);
    __m128 sum2 = _mm_set1_ps(0.0f);
    __m128 sum3 = _mm_set1_ps(0.0f);

    // r = 4*floor(n/16)
    unsigned int r = (n >> 4) << 2;

    //
    unsigned int i;
    for (i=0; i<r; i+=4) {
        // load inputs into register (unaligned)
        v0 = _mm_loadu_ps(&x[4*i+0]);
        v1 = _mm_loadu_ps(&x[4*i+4]);
        v2 = _mm_loadu_ps(&x[4*i+8]);
        v3 = _mm_loadu_ps(&x[4*i+12]);

        // load coefficients into register (unaligned)
        // TODO : ensure proper alignment
        h0 = _mm_loadu_ps(&_q->h[4*i+0]);
        h1 = _mm_loadu_ps(&_q->h[4*i+4]);
        h2 = _mm_loadu_ps(&_q->h[4*i+8]);
        h3 = _mm_loadu_ps(&_q->h[4*i+12]);

        // compute multiplication
        s0 = _mm_mul_ps(v0, h0);
        s1 = _mm_mul_ps(v1, h1);
        s2 = _mm_mul_ps(v2, h2);
        s3 = _mm_mul_ps(v3, h3);
        
        // parallel addition
        sum0 = _mm_add_ps( sum0, s0 );
        sum1 = _mm_add_ps( sum1, s1 );
        sum2 = _mm_add_ps( sum2, s2 );
        sum3 = _mm_add_ps( sum3, s3 );
    }

    // fold down
    sum0 = _mm_add_ps( sum0, sum1 );
    sum2 = _mm_add_ps( sum2, sum3 );
    union { __m128 v; float w[4] __attribute__((aligned(16)));} total;
    total.v = _mm_add_ps( sum0, sum2 );
    total.w[0] += total.w[2];
    total.w[1] += total.w[3];

    // cleanup (note: n _must_ be even)
    for (i=4*r; i<n; i+=2) {
        total.w[0] += x[i  ] * _q->h[i  ];
        total.w[1] += x[i+1] * _q->h[i+1];
    }

    // set return value
    *_y = total.w[0] + total.w[1]*_Complex_I;
}

