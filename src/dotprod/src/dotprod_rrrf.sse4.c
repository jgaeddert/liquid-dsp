/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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
// Floating-point dot product (SSE4.1/2)
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "liquid.internal.h"

// include proper SIMD extensions for x86 platforms
// NOTE: these pre-processor macros are defined in config.h

#if 0
#include <mmintrin.h>   // MMX
#include <xmmintrin.h>  // SSE
#include <emmintrin.h>  // SSE2
#include <pmmintrin.h>  // SSE3
#endif
#include <smmintrin.h>  // SSE4.1/2

#define DEBUG_DOTPROD_RRRF_SSE4     0

// internal methods
void dotprod_rrrf_execute_sse4(dotprod_rrrf _q, float * _x, float * _y);

// basic dot product (ordinal calculation)
void dotprod_rrrf_run(float *_h,
                      float *_x,
                      unsigned int _n,
                      float * _y)
{
    float r=0;
    unsigned int i;
    for (i=0; i<_n; i++)
        r += _h[i] * _x[i];
    *_y = r;
}

// basic dot product (ordinal calculation) with loop unrolled
void dotprod_rrrf_run4(float *_h,
                       float *_x,
                       unsigned int _n,
                       float * _y)
{
    float r=0;

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

struct dotprod_rrrf_s {
    unsigned int n;     // length
    float * h;          // coefficients array
};

dotprod_rrrf dotprod_rrrf_create(float * _h,
                                 unsigned int _n)
{
    dotprod_rrrf q = (dotprod_rrrf)malloc(sizeof(struct dotprod_rrrf_s));
    q->n = _n;

    // allocate memory for coefficients, 16-byte aligned
    q->h = (float*) _mm_malloc( q->n*sizeof(float), 16);

    // set coefficients
    memmove(q->h, _h, _n*sizeof(float));

    // return object
    return q;
}

// re-create the structured dotprod object
dotprod_rrrf dotprod_rrrf_recreate(dotprod_rrrf _dp,
                                   float * _h,
                                   unsigned int _n)
{
    // completely destroy and re-create dotprod object
    dotprod_rrrf_destroy(_dp);
    _dp = dotprod_rrrf_create(_h,_n);
    return _dp;
}


void dotprod_rrrf_destroy(dotprod_rrrf _q)
{
    _mm_free(_q->h);
    free(_q);
}

void dotprod_rrrf_print(dotprod_rrrf _q)
{
    printf("dotprod_rrrf:\n");
    unsigned int i;
    for (i=0; i<_q->n; i++)
        printf("%3u : %12.9f\n", i, _q->h[i]);
}

// 
void dotprod_rrrf_execute(dotprod_rrrf _q,
                          float * _x,
                          float * _y)
{
    dotprod_rrrf_execute_sse4(_q, _x, _y);
}

// use MMX/SSE extensions
void dotprod_rrrf_execute_sse4(dotprod_rrrf _q,
                              float * _x,
                              float * _y)
{
    __m128 v;   // input vector
    __m128 h;   // coefficients vector
    __m128 s;   // dot product
    __m128 sum = _mm_setzero_ps(); // load zeros into sum register

    // t = 4*(floor(_n/4))
    unsigned int t = (_q->n >> 2) << 2;

    //
    unsigned int i;
    for (i=0; i<t; i+=4) {
        // load inputs into register (unaligned)
        v = _mm_loadu_ps(&_x[i]);

        // load coefficients into register (aligned)
        h = _mm_load_ps(&_q->h[i]);

        // compute dot product
        s = _mm_dp_ps(v, h, 0xffffffff);
        
        // parallel addition
        sum = _mm_add_ps( sum, s );
    }

    // aligned output array
    float w[4] __attribute__((aligned(16)));

    // unload packed array
    _mm_store_ps(w, sum);
    float total = w[0];

    // cleanup
    for (; i<_q->n; i++)
        total += _x[i] * _q->h[i];

    // set return value
    *_y = total;
}

