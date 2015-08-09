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
// Floating-point dot product (MMX)
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "liquid.internal.h"

// include proper SIMD extensions for x86 platforms
// NOTE: these pre-processor macros are defined in config.h

#if HAVE_MMINTRIN_H
#include <mmintrin.h>   // MMX
#endif

#if HAVE_XMMINTRIN_H
#include <xmmintrin.h>  // SSE
#endif

#if HAVE_EMMINTRIN_H
#include <emmintrin.h>  // SSE2
#endif

#if HAVE_PMMINTRIN_H
#include <pmmintrin.h>  // SSE3
#endif

#define DEBUG_DOTPROD_RRRF_MMX   0

// internal methods
void dotprod_rrrf_execute_mmx(dotprod_rrrf _q,
                              float *      _x,
                              float *      _y);
void dotprod_rrrf_execute_mmx4(dotprod_rrrf _q,
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
                                   float * _h,
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
    printf("dotprod_rrrf [mmx, %u coefficients]\n", _q->n);
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
        dotprod_rrrf_execute_mmx(_q, _x, _y);
    } else {
        dotprod_rrrf_execute_mmx4(_q, _x, _y);
    }
}

// use MMX/SSE extensions
void dotprod_rrrf_execute_mmx(dotprod_rrrf _q,
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

#if HAVE_PMMINTRIN_H
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
}

// use MMX/SSE extensions, unrolled loop
void dotprod_rrrf_execute_mmx4(dotprod_rrrf _q,
                               float *      _x,
                               float *      _y)
{
    // first cut: ...
    __m128 v0, v1, v2, v3;
    __m128 h0, h1, h2, h3;
    __m128 s0, s1, s2, s3;

    // load zeros into sum registers
    __m128 sum0 = _mm_setzero_ps();
    __m128 sum1 = _mm_setzero_ps();
    __m128 sum2 = _mm_setzero_ps();
    __m128 sum3 = _mm_setzero_ps();

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
        sum0 = _mm_add_ps( sum0, s0 );
        sum1 = _mm_add_ps( sum1, s1 );
        sum2 = _mm_add_ps( sum2, s2 );
        sum3 = _mm_add_ps( sum3, s3 );
    }

    // fold down into single 4-element register
    sum0 = _mm_add_ps( sum0, sum1 );
    sum2 = _mm_add_ps( sum2, sum3 );
    sum0 = _mm_add_ps( sum0, sum2);

    // aligned output array
    float w[4] __attribute__((aligned(16)));

#if HAVE_PMMINTRIN_H
    // SSE3: fold down to single value using _mm_hadd_ps()
    __m128 z = _mm_setzero_ps();
    sum0 = _mm_hadd_ps(sum0, z);
    sum0 = _mm_hadd_ps(sum0, z);
   
    // unload single (lower value)
    _mm_store_ss(w, sum0);
    float total = w[0];
#else
    // SSE2 and below: unload packed array and perform manual sum
    _mm_store_ps(w, sum0);
    float total = w[0] + w[1] + w[2] + w[3];
#endif

    // cleanup
    // TODO : use intrinsics here as well
    for (i=4*r; i<_q->n; i++)
        total += _x[i] * _q->h[i];

    // set return value
    *_y = total;
}

