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
// Floating-point dot product (ARM Neon)
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.internal.h"

// build guard
#if BUILD_NEON

// include proper SIMD extensions for ARM Neon
#include <arm_neon.h>

// use ARM Neon extensions
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
int dotprod_cccf_execute_neon_1(dotprod_cccf    _q,
                                float complex * _x,
                                float complex * _y)
{
    // type cast input as floating point array
    float * x = (float*) _x;

    // double effective length
    unsigned int n = 2*_q->n;

    // temporary buffers
    float32x4_t v;   // input vector
    float32x4_t hi;  // coefficients vector (real)
    float32x4_t hq;  // coefficients vector (imag)
    float32x4_t ci;  // output multiplication (v * hi)
    float32x4_t cq;  // output multiplication (v * hq)

    // output accumulators
    float zeros[4] = {0,0,0,0};
    float32x4_t sumi = vld1q_f32(zeros);
    float32x4_t sumq = vld1q_f32(zeros);

    // t = 4*(floor(_n/4))
    unsigned int t = (n >> 2) << 2;

    //
    unsigned int i;
    for (i=0; i<t; i+=4) {
        // load inputs into register (unaligned)
        // {x[0].real, x[0].imag, x[1].real, x[1].imag}
        v = vld1q_f32(&x[i]);

        // load coefficients into register (aligned)
        // {hi[0].real, hi[0].real, hi[1].real, hi[1].real}
        // {hq[0].imag, hq[0].imag, hq[1].imag, hq[1].imag}
        hi = vld1q_f32(&_q->hi[i]);
        hq = vld1q_f32(&_q->hq[i]);

        // compute parallel multiplications
        ci = vmulq_f32(v, hi);
        cq = vmulq_f32(v, hq);

        // parallel addition
        sumi = vaddq_f32(sumi, ci);
        sumq = vaddq_f32(sumq, cq);
    }

    // unload and combine
    float wi[4];
    float wq[4];
    vst1q_f32(wi, sumi);
    vst1q_f32(wq, sumq);

    // fold down (add/sub)
    float complex total = 
        ((wi[0] - wq[1]) + (wi[2] - wq[3])) +
        ((wi[1] + wq[0]) + (wi[3] + wq[2])) * _Complex_I;

    // cleanup
    for (i=t/2; i<_q->n; i++)
        total += _x[i] * ( _q->hi[2*i] + _q->hq[2*i]*_Complex_I );

    // set return value
    *_y = total;
    return LIQUID_OK;
}

// use ARM Neon extensions (unrolled loop)
// NOTE: unrolling doesn't show any appreciable performance difference
int dotprod_cccf_execute_neon_4(dotprod_cccf    _q,
                                float complex * _x,
                                float complex * _y)
{
    // type cast input as floating point array
    float * x = (float*) _x;

    // double effective length
    unsigned int n = 2*_q->n;

    // first cut: ...
    float32x4_t v0,  v1,  v2,  v3;   // input vectors
    float32x4_t hi0, hi1, hi2, hi3;  // coefficients vectors (real)
    float32x4_t hq0, hq1, hq2, hq3;  // coefficients vectors (imag)
    float32x4_t ci0, ci1, ci2, ci3;  // output multiplications (v * hi)
    float32x4_t cq0, cq1, cq2, cq3;  // output multiplications (v * hq)

    // load zeros into sum registers
    float zeros[4] = {0,0,0,0};
    float32x4_t sumi = vld1q_f32(zeros);
    float32x4_t sumq = vld1q_f32(zeros);

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

        // load real coefficients into registers (aligned)
        hi0 = vld1q_f32(&_q->hi[4*i+0]);
        hi1 = vld1q_f32(&_q->hi[4*i+4]);
        hi2 = vld1q_f32(&_q->hi[4*i+8]);
        hi3 = vld1q_f32(&_q->hi[4*i+12]);

        // load real coefficients into registers (aligned)
        hq0 = vld1q_f32(&_q->hq[4*i+0]);
        hq1 = vld1q_f32(&_q->hq[4*i+4]);
        hq2 = vld1q_f32(&_q->hq[4*i+8]);
        hq3 = vld1q_f32(&_q->hq[4*i+12]);
        
        // compute parallel multiplications (real)
        ci0 = vmulq_f32(v0, hi0);
        ci1 = vmulq_f32(v1, hi1);
        ci2 = vmulq_f32(v2, hi2);
        ci3 = vmulq_f32(v3, hi3);

        // compute parallel multiplications (imag)
        cq0 = vmulq_f32(v0, hq0);
        cq1 = vmulq_f32(v1, hq1);
        cq2 = vmulq_f32(v2, hq2);
        cq3 = vmulq_f32(v3, hq3);

        // accumulate
        sumi = vaddq_f32(sumi, ci0);    sumq = vaddq_f32(sumq, cq0);
        sumi = vaddq_f32(sumi, ci1);    sumq = vaddq_f32(sumq, cq1);
        sumi = vaddq_f32(sumi, ci2);    sumq = vaddq_f32(sumq, cq2);
        sumi = vaddq_f32(sumi, ci3);    sumq = vaddq_f32(sumq, cq3);
    }

    // unload
    float wi[4];
    float wq[4];
    vst1q_f32(wi, sumi);
    vst1q_f32(wq, sumq);

    // fold down (add/sub)
    float complex total = 
        ((wi[0] - wq[1]) + (wi[2] - wq[3])) +
        ((wi[1] + wq[0]) + (wi[3] + wq[2])) * _Complex_I;

    // cleanup (note: n _must_ be even)
    // TODO : clean this method up
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
int dotprod_cccf_execute_neon(dotprod_cccf    _q,
                              float complex * _x,
                              float complex * _y)
{
    liquid_log_trace("dotprod_cccf_execute_neon()");
    // switch based on size
    if (_q->n < 32) {
        return dotprod_cccf_execute_neon_1(_q, _x, _y);
    }
    return dotprod_cccf_execute_neon_4(_q, _x, _y);
}


// build guard
#else

// invalidated
int dotprod_cccf_execute_neon(dotprod_cccf    _q,
                              float complex * _x,
                              float complex * _y)
{
    return liquid_error(LIQUID_EICONFIG,"neon extensions not available");
}

// build guard
#endif

