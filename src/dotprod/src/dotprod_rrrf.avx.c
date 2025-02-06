/*
 * Copyright (c) 2007 - 2025 Joseph Gaeddert
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
#include <assert.h>
#include <immintrin.h>
#include "liquid.internal.h"

#define DEBUG_DOTPROD_RRRF_AVX     0

// internal methods
int dotprod_rrrf_execute_avx(dotprod_rrrf _q,
                              float *      _x,
                              float *      _y);
int dotprod_rrrf_execute_avxu(dotprod_rrrf _q,
                               float *      _x,
                               float *      _y);

// basic dot product (ordinal calculation)
int dotprod_rrrf_run(float *      _h,
                     float *      _x,
                     unsigned int _n,
                     float *      _y)
{
    float r=0;
    unsigned int i;
    for (i=0; i<_n; i++)
        r += _h[i] * _x[i];
    *_y = r;
    return LIQUID_OK;
}

// basic dot product (ordinal calculation) with loop unrolled
int dotprod_rrrf_run4(float *      _h,
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
    return LIQUID_OK;
}


//
// structured AVX dot product
//

struct dotprod_rrrf_s {
    unsigned int n;     // length
    float * h;          // coefficients array
};

dotprod_rrrf dotprod_rrrf_create_opt(float *      _h,
                                     unsigned int _n,
                                     int          _rev)
{
    dotprod_rrrf q = (dotprod_rrrf)malloc(sizeof(struct dotprod_rrrf_s));
    q->n = _n;

    // allocate memory for coefficients, 32-byte aligned
    q->h = (float*) _mm_malloc( q->n*sizeof(float), 32);

    // set coefficients
    unsigned int i;
    for (i=0; i<q->n; i++)
        q->h[i] = _h[_rev ? q->n-i-1 : i];

    // return object
    return q;
}

dotprod_rrrf dotprod_rrrf_create(float *      _h,
                                 unsigned int _n)
{
    return dotprod_rrrf_create_opt(_h, _n, 0);
}

dotprod_rrrf dotprod_rrrf_create_rev(float *      _h,
                                     unsigned int _n)
{
    return dotprod_rrrf_create_opt(_h, _n, 1);
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

// re-create the structured dotprod object, coefficients reversed
dotprod_rrrf dotprod_rrrf_recreate_rev(dotprod_rrrf _q,
                                       float *      _h,
                                       unsigned int _n)
{
    // completely destroy and re-create dotprod object
    dotprod_rrrf_destroy(_q);
    return dotprod_rrrf_create_rev(_h,_n);
}

dotprod_rrrf dotprod_rrrf_copy(dotprod_rrrf q_orig)
{
    // validate input
    if (q_orig == NULL)
        return liquid_error_config("dotprod_rrrf_copy().avx, object cannot be NULL");

    dotprod_rrrf q_copy = (dotprod_rrrf)malloc(sizeof(struct dotprod_rrrf_s));
    q_copy->n = q_orig->n;

    // allocate memory for coefficients, 32-byte aligned
    q_copy->h = (float*) _mm_malloc( q_copy->n*sizeof(float), 32 );

    // copy coefficients array
    memmove(q_copy->h, q_orig->h, q_orig->n*sizeof(float));

    // return object
    return q_copy;
}

int dotprod_rrrf_destroy(dotprod_rrrf _q)
{
    _mm_free(_q->h);
    free(_q);
    return LIQUID_OK;
}

int dotprod_rrrf_print(dotprod_rrrf _q)
{
    printf("dotprod_rrrf [avx, %u coefficients]\n", _q->n);
    unsigned int i;
    for (i=0; i<_q->n; i++)
        printf("%3u : %12.9f\n", i, _q->h[i]);
    return LIQUID_OK;
}

// 
int dotprod_rrrf_execute(dotprod_rrrf _q,
                          float *      _x,
                          float *      _y)
{
    // switch based on size
    if (_q->n < 32) {
        return dotprod_rrrf_execute_avx(_q, _x, _y);
    }
    return dotprod_rrrf_execute_avxu(_q, _x, _y);
}

// use AVX extensions
int dotprod_rrrf_execute_avx(dotprod_rrrf _q,
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
int dotprod_rrrf_execute_avxu(dotprod_rrrf _q,
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

