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
    float * h_mem;      // allocated memory
    float * h;          // aligned coefficients array
};

dotprod_crcf dotprod_crcf_create(float * _h,
                                 unsigned int _n)
{
    dotprod_crcf q = (dotprod_crcf)malloc(sizeof(struct dotprod_crcf_s));
    q->n = _n;

    // allocate memory for coefficients
    q->h_mem = (float*) malloc( (2*q->n + 4)*sizeof(float) );

    // find alignment and set internal pointer
    unsigned int i;
    for (i=0; i<4; i++) {
        int align = ((long int)(&q->h_mem[i]) & 15)/sizeof(float);

        if (align == 0)
            q->h = &q->h_mem[i];
    }

    // assert( h is 16-byte aligned )
    int align = ((long int)(q->h) & 15)/sizeof(float);
    assert(align == 0);

    // set coefficients
#if 0
    memmove(q->h, _h, _n*sizeof(float));
#else
    // set coefficients, repeated
    //  h = { _h[0], _h[0], _h[1], _h[1], ... _h[n-1], _h[n-1]}
    for (i=0; i<q->n; i++) {
        q->h[2*i+0] = _h[i];
        q->h[2*i+1] = _h[i];
    }
#endif

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
    free(_q->h_mem);
    free(_q);
}

void dotprod_crcf_print(dotprod_crcf _q)
{
    printf("dotprod_crcf:\n");
    unsigned int i;
    // print coefficients to screen, skipping odd entries (repeated
    // coefficients)
    for (i=0; i<_q->n; i++)
        printf("%3u : %12.9f\n", i, _q->h[2*i]);
}

// 
void dotprod_crcf_execute(dotprod_crcf _q,
                          float complex * _x,
                          float complex * _y)
{
    dotprod_crcf_execute_mmx(_q, _x, _y);
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
    __m128 v;
    __m128 h;
    union { __m128 v; float w[4] __attribute__((aligned(16)));} s;
    float sum_i = 0.0f;
    float sum_q = 0.0f;

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
        s.v = _mm_mul_ps(v, h);

        // add into total (not necessarily efficient...)
        // TODO : fold into single element
        sum_i += s.w[0] + s.w[2];
        sum_q += s.w[1] + s.w[3];
    }

    // cleanup (note: n _must_ be even)
    for (; i<n; i+=2) {
        sum_i += x[i  ] * _q->h[i  ];
        sum_q += x[i+1] * _q->h[i+1];
    }

    // set return value
    *_y = sum_i + _Complex_I*sum_q;
}

