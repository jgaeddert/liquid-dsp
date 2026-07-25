/*
 * Copyright (c) 2007 - 2026 Joseph Gaeddert
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
// Floating-point dot product (SSE)
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.internal.h"

// build guard
#if BUILD_SSE

// include proper SIMD extensions for x86 SSE
#include <immintrin.h>

// use SSE extensions
int __attribute__((target("sse")))
dotprod_rrrf_execute_sse_1(dotprod_rrrf _q,
                               float *      _x,
                               float *      _y)
{
    // first cut: ...
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

        // compute multiplication
        s = _mm_mul_ps(v, h);

        // parallel addition
        sum = _mm_add_ps( sum, s );
    }

    // aligned output array
    float w[4] __attribute__((aligned(16)));

#if 0 // HAVE_SSE3
    // fold down into single value
    __m128 z = _mm_setzero_ps();
    sum = _mm_hadd_ps(sum, z);
    sum = _mm_hadd_ps(sum, z);
   
    // unload single (lower value)
    _mm_store_ss(w, sum);
    float total = w[0];
#else
    // unload packed array
    _mm_store_ps(w, sum);
    float total = w[0] + w[1] + w[2] + w[3];
#endif

    // cleanup
    for (; i<_q->n; i++)
        total += _x[i] * _q->h[i];

    // set return value
    *_y = total;
    return LIQUID_OK;
}

// use SSE extensions (unrolled loop)
int __attribute__((target("sse")))
dotprod_rrrf_execute_sse_4(dotprod_rrrf _q,
                               float *      _x,
                               float *      _y)
{
    // first cut: ...
    __m128 v0, v1, v2, v3;
    __m128 h0, h1, h2, h3;
    __m128 s0, s1, s2, s3;

    // load zeros into sum registers
    __m128 sum = _mm_setzero_ps();

    // r = 4*floor(n/16)
    unsigned int r = (_q->n >> 4) << 2;

    //
    unsigned int i;
    for (i=0; i<r; i+=4) {
        // load inputs into register (unaligned)
        v0 = _mm_loadu_ps(&_x[4*i+0]);
        v1 = _mm_loadu_ps(&_x[4*i+4]);
        v2 = _mm_loadu_ps(&_x[4*i+8]);
        v3 = _mm_loadu_ps(&_x[4*i+12]);

        // load coefficients into register (aligned)
        h0 = _mm_load_ps(&_q->h[4*i+0]);
        h1 = _mm_load_ps(&_q->h[4*i+4]);
        h2 = _mm_load_ps(&_q->h[4*i+8]);
        h3 = _mm_load_ps(&_q->h[4*i+12]);

        // compute multiplication
        s0 = _mm_mul_ps(v0, h0);
        s1 = _mm_mul_ps(v1, h1);
        s2 = _mm_mul_ps(v2, h2);
        s3 = _mm_mul_ps(v3, h3);
        
        // parallel addition
        sum = _mm_add_ps( sum, s0 );
        sum = _mm_add_ps( sum, s1 );
        sum = _mm_add_ps( sum, s2 );
        sum = _mm_add_ps( sum, s3 );
    }

    // aligned output array
    float w[4] __attribute__((aligned(16)));

#if HAVE_SSE3
    // SSE3: fold down to single value using _mm_hadd_ps()
    __m128 z = _mm_setzero_ps();
    sum = _mm_hadd_ps(sum, z);
    sum = _mm_hadd_ps(sum, z);
   
    // unload single (lower value)
    _mm_store_ss(w, sum);
    float total = w[0];
#else
    // SSE2 and below: unload packed array and perform manual sum
    _mm_store_ps(w, sum);
    float total = w[0] + w[1] + w[2] + w[3];
#endif

    // cleanup
    for (i=4*r; i<_q->n; i++)
        total += _x[i] * _q->h[i];

    // set return value
    *_y = total;
    return LIQUID_OK;
}

// execute structured dot product
//  _q      :   dotprod object
//  _x      :   input array
//  _y      :   output sample
int __attribute__((target("sse")))
dotprod_rrrf_execute_sse(dotprod_rrrf _q,
                         float *      _x,
                         float *      _y)
{
    liquid_log_trace("dotprod_rrrf_execute_sse()");
    // switch based on size
    if (_q->n < 16) {
        return dotprod_rrrf_execute_sse_1(_q, _x, _y);
    }
    return dotprod_rrrf_execute_sse_4(_q, _x, _y);
}

// build guard
#else

// invalidated
int dotprod_rrrf_execute_sse(dotprod_rrrf _q,
                             float *      _x,
                             float *      _y)
{
    return liquid_error(LIQUID_EICONFIG,"sse extensions not available");
}

// build guard
#endif

