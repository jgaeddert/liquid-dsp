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
// Floating-point dot product (AVX512-F)
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.internal.h"

// build guard
#if BUILD_AVX512

// include proper SIMD extensions for x86 AVX-512
#include <immintrin.h>

// use AVX512-F extensions
int __attribute__((target("avx512f,avx512dq,avx512vl,avx512bw,fma")))
dotprod_rrrf_execute_avx512_1(dotprod_rrrf _q,
                                  float *      _x,
                                  float *      _y)
{
    __m512 v;   // input vector
    __m512 h;   // coefficients vector
    __m512 s;   // dot product
    __m512 sum = _mm512_setzero_ps(); // load zeros into sum register

    // t = 16*(floor(_n/16))
    unsigned int t = (_q->n >> 4) << 4;

    //
    unsigned int i;
    for (i=0; i<t; i+=16) {
        // load inputs into register (unaligned)
        v = _mm512_loadu_ps(&_x[i]);

        // load coefficients into register (aligned)
        h = _mm512_load_ps(&_q->h[i]);

        // compute dot product
        s = _mm512_mul_ps(v, h);

        // parallel addition
        sum = _mm512_add_ps( sum, s );
    }

    // fold down into single value
    float total = _mm512_reduce_add_ps(sum);

    // cleanup
    for (; i<_q->n; i++)
        total += _x[i] * _q->h[i];

    // set return value
    *_y = total;
    return LIQUID_OK;
}

// use AVX512-F extensions (unrolled loop)
int __attribute__((target("avx512f,avx512dq,avx512vl,avx512bw,fma")))
dotprod_rrrf_execute_avx512_4(dotprod_rrrf _q,
                                  float *      _x,
                                  float *      _y)
{
    __m512 v0, v1, v2, v3;
    __m512 h0, h1, h2, h3;
    __m512 s0, s1, s2, s3;
    __m512 sum = _mm512_setzero_ps(); // load zeros into sum register

    // t = 16*(floor(_n/64))
    unsigned int r = (_q->n >> 6) << 4;

    //
    unsigned int i;
    for (i=0; i<r; i+=16) {
        // load inputs into register (unaligned)
        v0 = _mm512_loadu_ps(&_x[4*i+ 0]);
        v1 = _mm512_loadu_ps(&_x[4*i+16]);
        v2 = _mm512_loadu_ps(&_x[4*i+32]);
        v3 = _mm512_loadu_ps(&_x[4*i+48]);

        // load coefficients into register (aligned)
        h0 = _mm512_load_ps(&_q->h[4*i+ 0]);
        h1 = _mm512_load_ps(&_q->h[4*i+16]);
        h2 = _mm512_load_ps(&_q->h[4*i+32]);
        h3 = _mm512_load_ps(&_q->h[4*i+48]);

        // compute dot products
        s0 = _mm512_mul_ps(v0, h0);
        s1 = _mm512_mul_ps(v1, h1);
        s2 = _mm512_mul_ps(v2, h2);
        s3 = _mm512_mul_ps(v3, h3);
        
        // parallel addition
        sum = _mm512_add_ps( sum, s0 );
        sum = _mm512_add_ps( sum, s1 );
        sum = _mm512_add_ps( sum, s2 );
        sum = _mm512_add_ps( sum, s3 );
    }

    // fold down into single value
    float total = _mm512_reduce_add_ps(sum);

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
int __attribute__((target("avx512f,avx512dq,avx512vl,avx512bw,fma")))
dotprod_rrrf_execute_avx512(dotprod_rrrf _q,
                            float *      _x,
                            float *      _y)
{
    liquid_log_trace("dotprod_rrrf_execute_avx512()");
    // switch based on size
    if (_q->n < 64) {
        return dotprod_rrrf_execute_avx512_1(_q, _x, _y);
    }
    return dotprod_rrrf_execute_avx512_4(_q, _x, _y);
}

// build guard
#else

// invalidated
int dotprod_rrrf_execute_avx512(dotprod_rrrf _q,
                                float *      _x,
                                float *      _y)
{
    return liquid_error(LIQUID_EICONFIG,"avx512 extensions not available");
}

// build guard
#endif

