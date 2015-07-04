/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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
void dotprod_rrrf_execute_sse4(dotprod_rrrf _q,
                               float *      _x,
                               float *      _y);
void dotprod_rrrf_execute_sse4u(dotprod_rrrf _q,
                                float *      _x,
                                float *      _y);

// basic dot product (ordinal calculation)
void dotprod_rrrf_run(float *      _h,
                      float *      _x,
                      unsigned int _n,
                      float *      _y)
{
    float r=0;
    unsigned int i;
    for (i=0; i<_n; i++)
        r += _h[i] * _x[i];
    *_y = r;
}

// basic dot product (ordinal calculation) with loop unrolled
void dotprod_rrrf_run4(float *      _h,
                       float *      _x,
                       unsigned int _n,
                       float *      _y)
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

dotprod_rrrf dotprod_rrrf_create(float *      _h,
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
dotprod_rrrf dotprod_rrrf_recreate(dotprod_rrrf _q,
                                   float *      _h,
                                   unsigned int _n)
{
    // completely destroy and re-create dotprod object
    dotprod_rrrf_destroy(_q);
    return dotprod_rrrf_create(_h,_n);
}


void dotprod_rrrf_destroy(dotprod_rrrf _q)
{
    _mm_free(_q->h);
    free(_q);
}

void dotprod_rrrf_print(dotprod_rrrf _q)
{
    printf("dotprod_rrrf [sse4.1/4.2, %u coefficients]\n", _q->n);
    unsigned int i;
    for (i=0; i<_q->n; i++)
        printf("%3u : %12.9f\n", i, _q->h[i]);
}

// 
void dotprod_rrrf_execute(dotprod_rrrf _q,
                          float *      _x,
                          float *      _y)
{
    // switch based on size
    if (_q->n < 16) {
        dotprod_rrrf_execute_sse4(_q, _x, _y);
    } else {
        dotprod_rrrf_execute_sse4u(_q, _x, _y);
    }
}

// use MMX/SSE extensions
void dotprod_rrrf_execute_sse4(dotprod_rrrf _q,
                               float *      _x,
                               float *      _y)
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

// use MMX/SSE extensions (unrolled)
void dotprod_rrrf_execute_sse4u(dotprod_rrrf _q,
                                float *      _x,
                                float *      _y)
{
    __m128 v0, v1, v2, v3;
    __m128 h0, h1, h2, h3;
    __m128 s0, s1, s2, s3;
    __m128 sum = _mm_setzero_ps(); // load zeros into sum register

    // t = 4*(floor(_n/16))
    unsigned int r = (_q->n >> 4) << 2;

    //
    unsigned int i;
    for (i=0; i<r; i+=4) {
        // load inputs into register (unaligned)
        v0 = _mm_loadu_ps(&_x[4*i+ 0]);
        v1 = _mm_loadu_ps(&_x[4*i+ 4]);
        v2 = _mm_loadu_ps(&_x[4*i+ 8]);
        v3 = _mm_loadu_ps(&_x[4*i+12]);

        // load coefficients into register (aligned)
        h0 = _mm_load_ps(&_q->h[4*i+ 0]);
        h1 = _mm_load_ps(&_q->h[4*i+ 4]);
        h2 = _mm_load_ps(&_q->h[4*i+ 8]);
        h3 = _mm_load_ps(&_q->h[4*i+12]);

        // compute dot products
        s0 = _mm_dp_ps(v0, h0, 0xffffffff);
        s1 = _mm_dp_ps(v1, h1, 0xffffffff);
        s2 = _mm_dp_ps(v2, h2, 0xffffffff);
        s3 = _mm_dp_ps(v3, h3, 0xffffffff);
        
        // parallel addition
        // FIXME: these additions are by far the limiting factor
        sum = _mm_add_ps( sum, s0 );
        sum = _mm_add_ps( sum, s1 );
        sum = _mm_add_ps( sum, s2 );
        sum = _mm_add_ps( sum, s3 );
    }

    // aligned output array
    float w[4] __attribute__((aligned(16)));

    // unload packed array
    _mm_store_ps(w, sum);
    float total = w[0];

    // cleanup
    for (i=4*r; i<_q->n; i++)
        total += _x[i] * _q->h[i];

    // set return value
    *_y = total;
}

