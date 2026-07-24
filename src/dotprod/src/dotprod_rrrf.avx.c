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
// Floating-point dot product (AVX)
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.internal.h"

// build guard
#if BUILD_AVX

// include proper SIMD extensions for x86 AVX
#include <immintrin.h>

// use AVX extensions
int __attribute__((target("avx")))
dotprod_rrrf_execute_avx_1(dotprod_rrrf _q,
                               float *      _x,
                               float *      _y)
{
    __m256 v;   // input vector
    __m256 h;   // coefficients vector
    __m256 s;   // dot product
    __m256 sum = _mm256_setzero_ps(); // load zeros into sum register

    // t = 8*(floor(_n/8))
    unsigned int t = (_q->n >> 3) << 3;

    //
    unsigned int i;
    for (i=0; i<t; i+=8) {
        // load inputs into register (unaligned)
        v = _mm256_loadu_ps(&_x[i]);

        // load coefficients into register (aligned)
        h = _mm256_load_ps(&_q->h[i]);

        // compute dot product
        s = _mm256_mul_ps(v, h);

        // parallel addition
        sum = _mm256_add_ps( sum, s );
    }

    // fold down into single value
    __m256 z = _mm256_setzero_ps();
    sum = _mm256_hadd_ps(sum, z);
    sum = _mm256_hadd_ps(sum, z);

    // aligned output array
    float w[8] __attribute__((aligned(32)));

    // unload packed array
    _mm256_store_ps(w, sum);
    float total = w[0] + w[4];

    // cleanup
    for (; i<_q->n; i++)
        total += _x[i] * _q->h[i];

    // set return value
    *_y = total;
    return LIQUID_OK;
}

// use AVX extensions (unrolled)
int __attribute__((target("avx")))
dotprod_rrrf_execute_avx_4(dotprod_rrrf _q,
                               float *      _x,
                               float *      _y)
{
    __m256 v0, v1, v2, v3;
    __m256 h0, h1, h2, h3;
    __m256 s0, s1, s2, s3;
    __m256 sum = _mm256_setzero_ps(); // load zeros into sum register

    // t = 8*(floor(_n/32))
    unsigned int r = (_q->n >> 5) << 3;

    //
    unsigned int i;
    for (i=0; i<r; i+=8) {
        // load inputs into register (unaligned)
        v0 = _mm256_loadu_ps(&_x[4*i+ 0]);
        v1 = _mm256_loadu_ps(&_x[4*i+ 8]);
        v2 = _mm256_loadu_ps(&_x[4*i+16]);
        v3 = _mm256_loadu_ps(&_x[4*i+24]);

        // load coefficients into register (aligned)
        h0 = _mm256_load_ps(&_q->h[4*i+ 0]);
        h1 = _mm256_load_ps(&_q->h[4*i+ 8]);
        h2 = _mm256_load_ps(&_q->h[4*i+16]);
        h3 = _mm256_load_ps(&_q->h[4*i+24]);

        // compute dot products
        s0 = _mm256_mul_ps(v0, h0);
        s1 = _mm256_mul_ps(v1, h1);
        s2 = _mm256_mul_ps(v2, h2);
        s3 = _mm256_mul_ps(v3, h3);
        
        // parallel addition
        sum = _mm256_add_ps( sum, s0 );
        sum = _mm256_add_ps( sum, s1 );
        sum = _mm256_add_ps( sum, s2 );
        sum = _mm256_add_ps( sum, s3 );
    }

    // fold down into single value
    __m256 z = _mm256_setzero_ps();
    sum = _mm256_hadd_ps(sum, z);
    sum = _mm256_hadd_ps(sum, z);

    // aligned output array
    float w[8] __attribute__((aligned(32)));

    // unload packed array
    _mm256_store_ps(w, sum);
    float total = w[0] + w[4];

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
int __attribute__((target("avx")))
dotprod_rrrf_execute_avx(dotprod_rrrf _q,
                         float *      _x,
                         float *      _y)
{
    liquid_log_trace("dotprod_rrrf_execute_avx()");
    // switch based on size
    if (_q->n < 32) {
        return dotprod_rrrf_execute_avx_1(_q, _x, _y);
    }
    return dotprod_rrrf_execute_avx_4(_q, _x, _y);
}

// build guard
#else

// invalidated
int dotprod_rrrf_execute_avx(dotprod_rrrf _q,
                             float *      _x,
                             float *      _y)
{
    return liquid_error(LIQUID_EICONFIG,"avx extensions not available");
}

// build guard
#endif

