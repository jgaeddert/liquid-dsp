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
#include <xmmintrin.h>  // MMX
#include <pmmintrin.h>  // SSE3: _mm_hadd_ps
#include <assert.h>

#include "liquid.internal.h"

#define DEBUG_DOTPROD_RRRF_MMX   0

// internal methods
void dotprod_rrrf_execute_mmx(dotprod_rrrf _q, float * _x, float * _y);
void dotprod_rrrf_execute_mmx4(dotprod_rrrf _q, float * _x, float * _y);

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
    float * h_mem;      // allocated memory
    float * h;          // aligned coefficients array
};

dotprod_rrrf dotprod_rrrf_create(float * _h,
                                 unsigned int _n)
{
    dotprod_rrrf q = (dotprod_rrrf)malloc(sizeof(struct dotprod_rrrf_s));
    q->n = _n;

    // allocate memory for coefficients
    q->h_mem = (float*) malloc( (q->n + 4)*sizeof(float) );

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
    memmove(q->h, _h, _n*sizeof(float));

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
    free(_q->h_mem);
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
    // switch based on size
    if (_q->n < 16) {
        dotprod_rrrf_execute_mmx(_q, _x, _y);
    } else {
        dotprod_rrrf_execute_mmx4(_q, _x, _y);
    }
}

// use MMX/SSE extensions
void dotprod_rrrf_execute_mmx(dotprod_rrrf _q,
                              float * _x,
                              float * _y)
{
#if DEBUG_DOTPROD_RRRF_MMX
    // check alignment
    int align = ((long int)_x & 15)/sizeof(float);
    printf("align : %d\n", align);
#endif

    // initialize zeros constant array
    float zeros[4] __attribute__((aligned(16))) = {0.0f, 0.0f, 0.0f, 0.0f};

    // first cut: ...
    __m128 v;   // input vector
    __m128 h;   // coefficients vector
    __m128 s;   // dot product
    union { __m128 v; float w[4] __attribute__((aligned(16)));} sum;
    sum.v = _mm_load_ps(zeros); // load zeros into sum register

    // t = 4*(floor(_n/4))
    unsigned int t = (_q->n >> 2) << 2;

    //
    unsigned int i;
    for (i=0; i<t; i+=4) {
        // load inputs into register (unaligned)
        v = _mm_loadu_ps(&_x[i]);

        // load coefficients into register (unaligned)
        // TODO : ensure proper alignment
        h = _mm_loadu_ps(&_q->h[i]);

        // compute multiplication
        s = _mm_mul_ps(v, h);
       
        // parallel addition
        sum.v = _mm_add_ps( sum.v, s );
    }

    // fold down into single value
    __m128 z = _mm_load_ps(zeros);
    sum.v = _mm_hadd_ps(sum.v, z);
    sum.v = _mm_hadd_ps(sum.v, z);
    
    float total = sum.w[0];

    // cleanup
    for (; i<_q->n; i++)
        total += _x[i] * _q->h[i];

    // set return value
    *_y = total;
}

// use MMX/SSE extensions, unrolled loop
void dotprod_rrrf_execute_mmx4(dotprod_rrrf _q,
                               float * _x,
                               float * _y)
{
#if DEBUG_DOTPROD_RRRF_MMX
    // check alignment
    int align = ((long int)_x & 15)/sizeof(float);
    printf("align : %d\n", align);
#endif

    // initialize zeros constant array
    float zeros[4] __attribute__((aligned(16))) = {0.0f, 0.0f, 0.0f, 0.0f};

    // first cut: ...
    __m128 v0, v1, v2, v3;
    __m128 h0, h1, h2, h3;
    __m128 s0, s1, s2, s3;

    // load zeros into sum registers
    __m128 sum0 = _mm_load_ps(zeros);
    __m128 sum1 = _mm_load_ps(zeros);
    __m128 sum2 = _mm_load_ps(zeros);
    __m128 sum3 = _mm_load_ps(zeros);

    // r = floor(n/4)
    unsigned int r = (_q->n >> 2);

    // t = 4*r = 4*(floor(_n/4))
    unsigned int t = r << 2;

    //
    unsigned int i;
    for (i=0; i<r; i+=4) {
        // load inputs into register (unaligned)
        v0 = _mm_loadu_ps(&_x[4*i+0]);
        v1 = _mm_loadu_ps(&_x[4*i+4]);
        v2 = _mm_loadu_ps(&_x[4*i+8]);
        v3 = _mm_loadu_ps(&_x[4*i+12]);

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

    // fold down into single value
    union { __m128 v; float w[4] __attribute__((aligned(16)));} total;
    sum0 = _mm_add_ps( sum0, sum1 );
    sum2 = _mm_add_ps( sum2, sum3 );
    total.v = _mm_add_ps( sum0, sum2);
    total.v = _mm_hadd_ps( total.v, total.v );
    total.v = _mm_hadd_ps( total.v, total.v );

    // cleanup
    // TODO : use intrinsics here as well
    for (i=t; i<_q->n; i++)
        total.w[0] += _x[i] * _q->h[i];

    // set return value
    *_y = total.w[0];
}

