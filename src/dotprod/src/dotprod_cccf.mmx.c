/*
 * Copyright (c) 2012 Joseph Gaeddert
 * Copyright (c) 2012 Virginia Polytechnic Institute & State University
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
#include <xmmintrin.h>  // SSE
#include <pmmintrin.h>  // SSE3: _mm_addsub_ps

#include "liquid.internal.h"

#define DEBUG_DOTPROD_CCCF_MMX   0

// forward declaration of internal methods
void dotprod_cccf_execute_mmx(dotprod_cccf _q,
                              float complex * _x,
                              float complex * _y);
void dotprod_cccf_execute_mmx4(dotprod_cccf _q,
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
                                 unsigned int _n)
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
dotprod_cccf dotprod_cccf_recreate(dotprod_cccf _dp,
                                   float complex * _h,
                                   unsigned int _n)
{
    // completely destroy and re-create dotprod object
    dotprod_cccf_destroy(_dp);
    _dp = dotprod_cccf_create(_h,_n);
    return _dp;
}


void dotprod_cccf_destroy(dotprod_cccf _q)
{
    _mm_free(_q->hi);
    _mm_free(_q->hq);
    free(_q);
}

void dotprod_cccf_print(dotprod_cccf _q)
{
    printf("dotprod_cccf:\n");
}

// 
void dotprod_cccf_execute(dotprod_cccf _q,
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
void dotprod_cccf_execute_mmx(dotprod_cccf _q,
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
    __m128 s;   // dot product
    __m128 sum = _mm_setzero_ps(); // load zeros into sum register

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
        
        // combine
        // TODO : add SSE2 version
        s = _mm_addsub_ps( ci, cq );

        // accumulate
        sum = _mm_add_ps(sum, s);
    }

    // aligned output array
    float w[4] __attribute__((aligned(16)));

    // unload packed array
    _mm_store_ps(w, sum);

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
void dotprod_cccf_execute_mmx4(dotprod_cccf _q,
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
    __m128 s0,  s1,  s2,  s3;   // dot product results

    // load zeros into sum registers
    __m128 sum = _mm_setzero_ps();

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

        // shuffle values
        cq0 = _mm_shuffle_ps( cq0, cq0, _MM_SHUFFLE(2,3,0,1) );
        cq1 = _mm_shuffle_ps( cq1, cq1, _MM_SHUFFLE(2,3,0,1) );
        cq2 = _mm_shuffle_ps( cq2, cq2, _MM_SHUFFLE(2,3,0,1) );
        cq3 = _mm_shuffle_ps( cq3, cq3, _MM_SHUFFLE(2,3,0,1) );
        
        // combine
        // TODO : add SSE2 version
        s0 = _mm_addsub_ps(ci0, cq0);
        s1 = _mm_addsub_ps(ci1, cq1);
        s2 = _mm_addsub_ps(ci2, cq2);
        s3 = _mm_addsub_ps(ci3, cq3);

        // accumulate
        sum = _mm_add_ps(sum, s0);
        sum = _mm_add_ps(sum, s1);
        sum = _mm_add_ps(sum, s2);
        sum = _mm_add_ps(sum, s3);
    }

    // unload
    float w[4] __attribute__((aligned(16)));
    _mm_store_ps(w, sum);

    // fold down
    w[0] += w[2];
    w[1] += w[3];

    //float complex total = *((float complex*)w);
    float complex total = w[0] + w[1] * _Complex_I;

    // cleanup (note: n _must_ be even)
    // TODO : clean this method up
    for (i=2*r; i<_q->n; i++) {
        total += _x[i] * ( _q->hi[2*i] + _q->hq[2*i]*_Complex_I );
    }

    // set return value
    *_y = total;
}

