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
//
// (a + jb)(c + jd) = (ac - bd) + j(ad + bc)
//
// mm_x  = { x[0].real, x[0].imag, x[1].real, x[1].imag, x[2].real, x[2].imag, x[3].real, x[3].imag }
// mm_hi = { h[0].real, h[0].real, h[1].real, h[1].real, h[2].real, h[2].real, h[3].real, h[3].real }
// mm_hq = { h[0].imag, h[0].imag, h[1].imag, h[1].imag, h[2].imag, h[2].imag, h[3].imag, h[3].imag }
//
// mm_y0 = mm_x * mm_hi
//       = { x[0].real * h[0].real,
//           x[0].imag * h[0].real,
//           x[1].real * h[1].real,
//           x[1].imag * h[1].real,
//           x[2].real * h[2].real,
//           x[2].imag * h[2].real,
//           x[3].real * h[3].real,
//           x[3].imag * h[3].real };
//
// mm_y1 = mm_x * mm_hq
//       = { x[0].real * h[0].imag,
//           x[0].imag * h[0].imag,
//           x[1].real * h[1].imag,
//           x[1].imag * h[1].imag,
//           x[2].real * h[2].imag,
//           x[2].imag * h[2].imag,
//           x[3].real * h[3].imag,
//           x[3].imag * h[3].imag };
//
int __attribute__((target("avx")))
dotprod_cccf_execute_avx_1(dotprod_cccf    _q,
                           float complex * _x,
                           float complex * _y)
{
    // type cast input as floating point array
    float * x = (float*) _x;

    // double effective length
    unsigned int n = 2*_q->n;

    // temporary buffers
    __m256 v;   // input vector
    __m256 hi;  // coefficients vector (real)
    __m256 hq;  // coefficients vector (imag)
    __m256 ci;  // output multiplication (v * hi)
    __m256 cq;  // output multiplication (v * hq)

    // aligned output array
    float w[8] __attribute__((aligned(32))) = {0,0,0,0,0,0,0,0};

    __m256 s;   // dot product
    __m256 sum = _mm256_setzero_ps(); // load zeros into sum register

    // t = 8*(floor(_n/8))
    unsigned int t = (n >> 3) << 3;

    //
    unsigned int i;
    for (i=0; i<t; i+=8) {
        // load inputs into register (unaligned)
        // {x[0].real, x[0].imag, x[1].real, x[1].imag, x[2].real, x[2].imag, x[3].real, x[3].imag}
        v = _mm256_loadu_ps(&x[i]);

        // load coefficients into register (aligned)
        hi = _mm256_load_ps(&_q->hi[i]);
        hq = _mm256_load_ps(&_q->hq[i]);

        // compute parallel multiplications
        ci = _mm256_mul_ps(v, hi);
        cq = _mm256_mul_ps(v, hq);

        // shuffle values
        cq = _mm256_shuffle_ps( cq, cq, _MM_SHUFFLE(2,3,0,1) );

        // combine using addsub_ps()
        s = _mm256_addsub_ps( ci, cq );

        // accumulate
        sum = _mm256_add_ps(sum, s);
    }

    // unload packed array
    _mm256_store_ps(w, sum);

    // add in-phase and quadrature components
    w[0] += w[2] + w[4] + w[6];   // I
    w[1] += w[3] + w[5] + w[7];   // Q

    float complex total = w[0] + w[1] * _Complex_I;

    // cleanup
    for (i=t/2; i<_q->n; i++)
        total += _x[i] * ( _q->hi[2*i] + _q->hq[2*i]*_Complex_I );

    // set return value
    *_y = total;
    return LIQUID_OK;
}

// use AVX extensions (unrolled loop)
int __attribute__((target("avx")))
dotprod_cccf_execute_avx_4(dotprod_cccf    _q,
                           float complex * _x,
                           float complex * _y)
{
    // type cast input as floating point array
    float * x = (float*) _x;

    // double effective length
    unsigned int n = 2*_q->n;

    // first cut: ...
    __m256 v0,  v1,  v2,  v3;   // input vectors
    __m256 hi0, hi1, hi2, hi3;  // coefficients vectors (real)
    __m256 hq0, hq1, hq2, hq3;  // coefficients vectors (imag)
    __m256 ci0, ci1, ci2, ci3;  // output multiplications (v * hi)
    __m256 cq0, cq1, cq2, cq3;  // output multiplications (v * hq)

    // load zeros into sum registers
    __m256 sumi = _mm256_setzero_ps();
    __m256 sumq = _mm256_setzero_ps();

    // r = 8*floor(n/32)
    unsigned int r = (n >> 5) << 3;

    //
    unsigned int i;
    for (i=0; i<r; i+=8) {
        // load inputs into register (unaligned)
        v0 = _mm256_loadu_ps(&x[4*i+0]);
        v1 = _mm256_loadu_ps(&x[4*i+8]);
        v2 = _mm256_loadu_ps(&x[4*i+16]);
        v3 = _mm256_loadu_ps(&x[4*i+24]);

        // load real coefficients into registers (aligned)
        hi0 = _mm256_load_ps(&_q->hi[4*i+0]);
        hi1 = _mm256_load_ps(&_q->hi[4*i+8]);
        hi2 = _mm256_load_ps(&_q->hi[4*i+16]);
        hi3 = _mm256_load_ps(&_q->hi[4*i+24]);

        // load real coefficients into registers (aligned)
        hq0 = _mm256_load_ps(&_q->hq[4*i+0]);
        hq1 = _mm256_load_ps(&_q->hq[4*i+8]);
        hq2 = _mm256_load_ps(&_q->hq[4*i+16]);
        hq3 = _mm256_load_ps(&_q->hq[4*i+24]);
        
        // compute parallel multiplications (real)
        ci0 = _mm256_mul_ps(v0, hi0);
        ci1 = _mm256_mul_ps(v1, hi1);
        ci2 = _mm256_mul_ps(v2, hi2);
        ci3 = _mm256_mul_ps(v3, hi3);

        // compute parallel multiplications (imag)
        cq0 = _mm256_mul_ps(v0, hq0);
        cq1 = _mm256_mul_ps(v1, hq1);
        cq2 = _mm256_mul_ps(v2, hq2);
        cq3 = _mm256_mul_ps(v3, hq3);

        // accumulate
        sumi = _mm256_add_ps(sumi, ci0);   sumq = _mm256_add_ps(sumq, cq0);
        sumi = _mm256_add_ps(sumi, ci1);   sumq = _mm256_add_ps(sumq, cq1);
        sumi = _mm256_add_ps(sumi, ci2);   sumq = _mm256_add_ps(sumq, cq2);
        sumi = _mm256_add_ps(sumi, ci3);   sumq = _mm256_add_ps(sumq, cq3);
    }

    // shuffle values
    sumq = _mm256_shuffle_ps( sumq, sumq, _MM_SHUFFLE(2,3,0,1) );

    // unload
    float wi[8] __attribute__((aligned(32)));
    float wq[8] __attribute__((aligned(32)));
    _mm256_store_ps(wi, sumi);
    _mm256_store_ps(wq, sumq);

    // fold down (add/sub)
    float complex total = 
        ((wi[0] - wq[0]) + (wi[2] - wq[2]) + (wi[4] - wq[4]) + (wi[6] - wq[6])) +
        ((wi[1] + wq[1]) + (wi[3] + wq[3]) + (wi[5] + wq[5]) + (wi[7] + wq[7])) * _Complex_I;

    // cleanup (note: n _must_ be even)
    for (i=2*r; i<_q->n; i++) {
        total += _x[i] * ( _q->hi[2*i] + _q->hq[2*i]*_Complex_I );
    }

    // set return value
    *_y = total;
    return LIQUID_OK;
}

// execute structured dot product
//  _q      :   dotprod object
//  _x      :   input array
//  _y      :   output sample
int __attribute__((target("avx")))
dotprod_cccf_execute_avx(dotprod_cccf    _q,
                         float complex * _x,
                         float complex * _y)
{
    liquid_log_trace("dotprod_cccf_execute_avx()");
    // switch based on size
    if (_q->n < 64) {
        return dotprod_cccf_execute_avx_1(_q, _x, _y);
    }
    return dotprod_cccf_execute_avx_4(_q, _x, _y);
}

// build guard
#else

// invalidated
int dotprod_cccf_execute_avx(dotprod_cccf    _q,
                             float complex * _x,
                             float complex * _y)
{
    return liquid_error(LIQUID_EICONFIG,"avx extensions not available");
}

// build guard
#endif

