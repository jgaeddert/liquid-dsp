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
// Floating-point dot product (ARM Neon)
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "liquid.internal.h"

// include proper SIMD extensions for ARM Neon
#include <arm_neon.h>

#define DEBUG_DOTPROD_CRCF_NEON   0

// forward declaration of internal methods
void dotprod_crcf_execute_neon(dotprod_crcf    _q,
                               float complex * _x,
                               float complex * _y);
void dotprod_crcf_execute_neon4(dotprod_crcf    _q,
                                float complex * _x,
                                float complex * _y);

// basic dot product (ordinal calculation) using neon extensions
void dotprod_crcf_run(float *         _h,
                      float complex * _x,
                      unsigned int    _n,
                      float complex * _y)
{
    // initialize accumulator
    float complex r=0;

    unsigned int i;
    for (i=0; i<_n; i++)
        r += _h[i] * _x[i];

    // return result
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
// structured ARM Neon dot product
//

struct dotprod_crcf_s {
    unsigned int n;     // length
    float * h;          // coefficients array
};

dotprod_crcf dotprod_crcf_create(float *      _h,
                                 unsigned int _n)
{
    dotprod_crcf q = (dotprod_crcf)malloc(sizeof(struct dotprod_crcf_s));
    q->n = _n;

    // allocate memory for coefficients (double size)
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
dotprod_crcf dotprod_crcf_recreate(dotprod_crcf _q,
                                   float *      _h,
                                   unsigned int _n)
{
    // completely destroy and re-create dotprod object
    dotprod_crcf_destroy(_q);
    return dotprod_crcf_create(_h,_n);
}


void dotprod_crcf_destroy(dotprod_crcf _q)
{
    // free coefficients array
    free(_q->h);

    // free main memory
    free(_q);
}

void dotprod_crcf_print(dotprod_crcf _q)
{
    // print coefficients to screen, skipping odd entries (due
    // to repeated coefficients)
    printf("dotprod_crcf [arm-neon, %u coefficients]\n", _q->n);
    unsigned int i;
    for (i=0; i<_q->n; i++)
        printf("  %3u : %12.9f\n", i, _q->h[2*i]);
}

// 
void dotprod_crcf_execute(dotprod_crcf    _q,
                          float complex * _x,
                          float complex * _y)
{
    // switch based on size
    if (_q->n < 32) {
        dotprod_crcf_execute_neon(_q, _x, _y);
    } else {
        dotprod_crcf_execute_neon4(_q, _x, _y);
    }
}

// use ARM Neon extensions
void dotprod_crcf_execute_neon(dotprod_crcf    _q,
                               float complex * _x,
                               float complex * _y)
{
    // type cast input as floating point array
    float * x = (float*) _x;

    // double effective length
    unsigned int n = 2*_q->n;

    // first cut: ...
    float32x4_t v;   // input vector
    float32x4_t h;   // coefficients vector
    float32x4_t s;   // dot product
    
    // load zeros into sum register
    float zeros[4] = {0,0,0,0};
    float32x4_t sum = vld1q_f32(zeros);

    // t = 4*(floor(_n/4))
    unsigned int t = (n >> 2) << 2;

    //
    unsigned int i;
    for (i=0; i<t; i+=4) {
        // load inputs into register (unaligned)
        v = vld1q_f32(&x[i]);

        // load coefficients into register (aligned)
        h = vld1q_f32(&_q->h[i]);

        // compute multiplication
        s = vmulq_f32(h,v);

        // accumulate
        sum = vaddq_f32(sum, s);
    }

    // unload packed array
    float w[4];
    vst1q_f32(w, sum);

    // add in-phase and quadrature components
    w[0] += w[2];
    w[1] += w[3];

    // cleanup (note: n _must_ be even)
    for (; i<n; i+=2) {
        w[0] += x[i  ] * _q->h[i  ];
        w[1] += x[i+1] * _q->h[i+1];
    }

    // set return value
    *_y = w[0] + _Complex_I*w[1];
}

// use ARM Neon extensions
void dotprod_crcf_execute_neon4(dotprod_crcf    _q,
                                float complex * _x,
                                float complex * _y)
{
#if 1
    // type cast input as floating point array
    float * x = (float*) _x;

    // double effective length
    unsigned int n = 2*_q->n;

    // first cut: ...
    float32x4_t v0, v1, v2, v3;  // input vectors
    float32x4_t h0, h1, h2, h3;  // coefficients vectors
    float32x4_t s0, s1, s2, s3;  // dot products [re, im, re, im]

    // load zeros into sum registers
    float zeros[4] = {0,0,0,0};
    float32x4_t sum0 = vld1q_f32(zeros);
    float32x4_t sum1 = vld1q_f32(zeros);
    float32x4_t sum2 = vld1q_f32(zeros);
    float32x4_t sum3 = vld1q_f32(zeros);

    // r = 4*floor(n/16)
    unsigned int r = (n >> 4) << 2;

    //
    unsigned int i;
    for (i=0; i<r; i+=4) {
        // load inputs into register (unaligned)
        v0 = vld1q_f32(&x[4*i+0]);
        v1 = vld1q_f32(&x[4*i+4]);
        v2 = vld1q_f32(&x[4*i+8]);
        v3 = vld1q_f32(&x[4*i+12]);

        // load coefficients into register (aligned)
        h0 = vld1q_f32(&_q->h[4*i+0]);
        h1 = vld1q_f32(&_q->h[4*i+4]);
        h2 = vld1q_f32(&_q->h[4*i+8]);
        h3 = vld1q_f32(&_q->h[4*i+12]);

        // compute multiplication
        s0 = vmulq_f32(v0, h0);
        s1 = vmulq_f32(v1, h1);
        s2 = vmulq_f32(v2, h2);
        s3 = vmulq_f32(v3, h3);
        
        // parallel addition
        sum0 = vaddq_f32( sum0, s0 );
        sum1 = vaddq_f32( sum1, s1 );
        sum2 = vaddq_f32( sum2, s2 );
        sum3 = vaddq_f32( sum3, s3 );
    }

    // fold down into sum0
    sum0 = vaddq_f32( sum0, sum1 );
    sum2 = vaddq_f32( sum2, sum3 );
    sum0 = vaddq_f32( sum0, sum2 );

    // unload packed array
    float w[4];
    vst1q_f32(w, sum0);

    // add in-phase and quadrature components
    w[0] += w[2];
    w[1] += w[3];

    // cleanup (note: n _must_ be even)
    for (i=4*r; i<n; i+=2) {
        w[0] += x[i  ] * _q->h[i  ];
        w[1] += x[i+1] * _q->h[i+1];
    }

    // set return value
    *_y = w[0] + w[1]*_Complex_I;
#else
    dotprod_crcf_execute_neon(_q, _x, _y);
#endif
}

