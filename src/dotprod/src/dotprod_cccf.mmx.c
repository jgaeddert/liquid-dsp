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

#include "liquid.internal.h"

// include proper SIMD extensions for x86 platforms
// NOTE: these pre-processor macros are defined in config.h

#if HAVE_MMX && HAVE_MMINTRIN_H
#include <mmintrin.h>   // MMX
#endif

#if HAVE_SSE && HAVE_XMMINTRIN_H
#include <xmmintrin.h>  // SSE
#endif

#if HAVE_SSE2 && HAVE_EMMINTRIN_H
#include <emmintrin.h>  // SSE2
#endif

#if HAVE_SSE3 && HAVE_PMMINTRIN_H
#include <pmmintrin.h>  // SSE3
#endif

#define DEBUG_DOTPROD_CCCF_MMX   0

// forward declaration of internal methods
void dotprod_cccf_execute_mmx(dotprod_cccf    _q,
                              float complex * _x,
                              float complex * _y);

void dotprod_cccf_execute_mmx4(dotprod_cccf    _q,
                               float complex * _x,
                               float complex * _y);

// basic dot product (ordinal calculation)
void dotprod_cccf_run(float complex * _h,
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
void dotprod_cccf_run4(float complex * _h,
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

struct dotprod_cccf_s {
    unsigned int n;     // length
    float * hi;         // in-phase
    float * hq;         // quadrature
};

dotprod_cccf dotprod_cccf_create(float complex * _h,
                                 unsigned int    _n)
{
    dotprod_cccf q = (dotprod_cccf)malloc(sizeof(struct dotprod_cccf_s));
    q->n = _n;

    // allocate memory for coefficients, 16-byte aligned
    q->hi = (float*) _mm_malloc( 2*q->n*sizeof(float), 16 );
    q->hq = (float*) _mm_malloc( 2*q->n*sizeof(float), 16 );

    // set coefficients, repeated
    //  hi = { crealf(_h[0]), crealf(_h[0]), ... crealf(_h[n-1]), crealf(_h[n-1])}
    //  hq = { cimagf(_h[0]), cimagf(_h[0]), ... cimagf(_h[n-1]), cimagf(_h[n-1])}
    unsigned int i;
    for (i=0; i<q->n; i++) {
        q->hi[2*i+0] = crealf(_h[i]);
        q->hi[2*i+1] = crealf(_h[i]);

        q->hq[2*i+0] = cimagf(_h[i]);
        q->hq[2*i+1] = cimagf(_h[i]);
    }

    // return object
    return q;
}

// re-create the structured dotprod object
dotprod_cccf dotprod_cccf_recreate(dotprod_cccf    _q,
                                   float complex * _h,
                                   unsigned int    _n)
{
    // completely destroy and re-create dotprod object
    dotprod_cccf_destroy(_q);
    return dotprod_cccf_create(_h,_n);
}


void dotprod_cccf_destroy(dotprod_cccf _q)
{
    _mm_free(_q->hi);
    _mm_free(_q->hq);
    free(_q);
}

void dotprod_cccf_print(dotprod_cccf _q)
{
    printf("dotprod_cccf [mmx, %u coefficients]\n", _q->n);
    unsigned int i;
    for (i=0; i<_q->n; i++)
        printf("  %3u : %12.9f +j%12.9f\n", i, _q->hi[i], _q->hq[i]);
}

// execute structured dot product
//  _q      :   dotprod object
//  _x      :   input array
//  _y      :   output sample
void dotprod_cccf_execute(dotprod_cccf    _q,
                          float complex * _x,
                          float complex * _y)
{
    // switch based on size
    if (_q->n < 32) {
        dotprod_cccf_execute_mmx(_q, _x, _y);
    } else {
        dotprod_cccf_execute_mmx4(_q, _x, _y);
    }
}

// use MMX/SSE extensions
//
// (a + jb)(c + jd) = (ac - bd) + j(ad + bc)
//
// mm_x  = { x[0].real, x[0].imag, x[1].real, x[1].imag }
// mm_hi = { h[0].real, h[0].real, h[1].real, h[1].real }
// mm_hq = { h[0].imag, h[0].imag, h[1].imag, h[1].imag }
//
// mm_y0 = mm_x * mm_hi
//       = { x[0].real * h[0].real,
//           x[0].imag * h[0].real,
//           x[1].real * h[1].real,
//           x[1].imag * h[1].real };
//
// mm_y1 = mm_x * mm_hq
//       = { x[0].real * h[0].imag,
//           x[0].imag * h[0].imag,
//           x[1].real * h[1].imag,
//           x[1].imag * h[1].imag };
//
void dotprod_cccf_execute_mmx(dotprod_cccf    _q,
                              float complex * _x,
                              float complex * _y)
{
    // type cast input as floating point array
    float * x = (float*) _x;

    // double effective length
    unsigned int n = 2*_q->n;

    // temporary buffers
    __m128 v;   // input vector
    __m128 hi;  // coefficients vector (real)
    __m128 hq;  // coefficients vector (imag)
    __m128 ci;  // output multiplication (v * hi)
    __m128 cq;  // output multiplication (v * hq)

    // aligned output array
    float w[4] __attribute__((aligned(16))) = {0,0,0,0};

#if HAVE_SSE3 && HAVE_PMMINTRIN_H
    // SSE3
    __m128 s;   // dot product
    __m128 sum = _mm_setzero_ps(); // load zeros into sum register
#else
    // no SSE3
    float wi[4] __attribute__((aligned(16)));
    float wq[4] __attribute__((aligned(16)));
#endif

    // t = 4*(floor(_n/4))
    unsigned int t = (n >> 2) << 2;

    //
    unsigned int i;
    for (i=0; i<t; i+=4) {
        // load inputs into register (unaligned)
        // {x[0].real, x[0].imag, x[1].real, x[1].imag}
        v = _mm_loadu_ps(&x[i]);

        // load coefficients into register (aligned)
        hi = _mm_load_ps(&_q->hi[i]);
        hq = _mm_load_ps(&_q->hq[i]);

        // compute parallel multiplications
        ci = _mm_mul_ps(v, hi);
        cq = _mm_mul_ps(v, hq);

        // shuffle values
        cq = _mm_shuffle_ps( cq, cq, _MM_SHUFFLE(2,3,0,1) );
        
#if HAVE_SSE3 && HAVE_PMMINTRIN_H
        // SSE3: combine using addsub_ps()
        s = _mm_addsub_ps( ci, cq );

        // accumulate
        sum = _mm_add_ps(sum, s);
#else
        // no SSE3: combine using slow method
        // FIXME: implement slow method
        // unload values
        _mm_store_ps(wi, ci);
        _mm_store_ps(wq, cq);

        // accumulate
        w[0] += wi[0] - wq[0];
        w[1] += wi[1] + wq[1];
        w[2] += wi[2] - wq[2];
        w[3] += wi[3] + wq[3];
#endif
    }

#if HAVE_SSE3 && HAVE_PMMINTRIN_H
    // unload packed array
    _mm_store_ps(w, sum);
#endif

    // add in-phase and quadrature components
    w[0] += w[2];   // I
    w[1] += w[3];   // Q

    //float complex total = *((float complex*)w);
    float complex total = w[0] + w[1] * _Complex_I;

    // cleanup
    for (i=t/2; i<_q->n; i++)
        total += _x[i] * ( _q->hi[2*i] + _q->hq[2*i]*_Complex_I );

    // set return value
    *_y = total;
}

// use MMX/SSE extensions
void dotprod_cccf_execute_mmx4(dotprod_cccf    _q,
                               float complex * _x,
                               float complex * _y)
{
    // type cast input as floating point array
    float * x = (float*) _x;

    // double effective length
    unsigned int n = 2*_q->n;

    // first cut: ...
    __m128 v0,  v1,  v2,  v3;   // input vectors
    __m128 hi0, hi1, hi2, hi3;  // coefficients vectors (real)
    __m128 hq0, hq1, hq2, hq3;  // coefficients vectors (imag)
    __m128 ci0, ci1, ci2, ci3;  // output multiplications (v * hi)
    __m128 cq0, cq1, cq2, cq3;  // output multiplications (v * hq)

    // load zeros into sum registers
    __m128 sumi = _mm_setzero_ps();
    __m128 sumq = _mm_setzero_ps();

    // r = 4*floor(n/16)
    unsigned int r = (n >> 4) << 2;

    //
    unsigned int i;
    for (i=0; i<r; i+=4) {
        // load inputs into register (unaligned)
        v0 = _mm_loadu_ps(&x[4*i+0]);
        v1 = _mm_loadu_ps(&x[4*i+4]);
        v2 = _mm_loadu_ps(&x[4*i+8]);
        v3 = _mm_loadu_ps(&x[4*i+12]);

        // load real coefficients into registers (aligned)
        hi0 = _mm_load_ps(&_q->hi[4*i+0]);
        hi1 = _mm_load_ps(&_q->hi[4*i+4]);
        hi2 = _mm_load_ps(&_q->hi[4*i+8]);
        hi3 = _mm_load_ps(&_q->hi[4*i+12]);

        // load real coefficients into registers (aligned)
        hq0 = _mm_load_ps(&_q->hq[4*i+0]);
        hq1 = _mm_load_ps(&_q->hq[4*i+4]);
        hq2 = _mm_load_ps(&_q->hq[4*i+8]);
        hq3 = _mm_load_ps(&_q->hq[4*i+12]);
        
        // compute parallel multiplications (real)
        ci0 = _mm_mul_ps(v0, hi0);
        ci1 = _mm_mul_ps(v1, hi1);
        ci2 = _mm_mul_ps(v2, hi2);
        ci3 = _mm_mul_ps(v3, hi3);

        // compute parallel multiplications (imag)
        cq0 = _mm_mul_ps(v0, hq0);
        cq1 = _mm_mul_ps(v1, hq1);
        cq2 = _mm_mul_ps(v2, hq2);
        cq3 = _mm_mul_ps(v3, hq3);

        // accumulate
        sumi = _mm_add_ps(sumi, ci0);   sumq = _mm_add_ps(sumq, cq0);
        sumi = _mm_add_ps(sumi, ci1);   sumq = _mm_add_ps(sumq, cq1);
        sumi = _mm_add_ps(sumi, ci2);   sumq = _mm_add_ps(sumq, cq2);
        sumi = _mm_add_ps(sumi, ci3);   sumq = _mm_add_ps(sumq, cq3);
    }

    // shuffle values
    sumq = _mm_shuffle_ps( sumq, sumq, _MM_SHUFFLE(2,3,0,1) );

    // unload
    float wi[4] __attribute__((aligned(16)));
    float wq[4] __attribute__((aligned(16)));
    _mm_store_ps(wi, sumi);
    _mm_store_ps(wq, sumq);

    // fold down (add/sub)
    float complex total = 
        ((wi[0] - wq[0]) + (wi[2] - wq[2])) +
        ((wi[1] + wq[1]) + (wi[3] + wq[3])) * _Complex_I;

    // cleanup (note: n _must_ be even)
    // TODO : clean this method up
    for (i=2*r; i<_q->n; i++) {
        total += _x[i] * ( _q->hi[2*i] + _q->hq[2*i]*_Complex_I );
    }

    // set return value
    *_y = total;
}

