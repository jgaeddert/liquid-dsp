/*
 * Copyright (c) 2007, 2009, 2012 Joseph Gaeddert
 * Copyright (c) 2007, 2009, 2012 Virginia Polytechnic Institute & State University
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

// 
// Fixed-point dot product (MMX)
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

#if HAVE_TMMINTRIN_H
#include <tmmintrin.h>  // SSSE3
#endif

#define DEBUG_DOTPROD_CCCQ16_MMX   0

#if DEBUG_DOTPROD_CCCQ16_MMX
void _mm_printq16_epi16(__m128i _v) {
    q16_t v[8] __attribute__((aligned(16)));
    _mm_store_si128((__m128i*)v, _v);
    printf("{");
    unsigned int i;
#if 0
    for (i=0; i<8; i++) printf(" %8.4f", q16_fixed_to_float(v[i]));
#else
    for (i=0; i<8; i++) printf(" %8d", v[i]);
#endif
    printf("}");
}

void _mm_printq16_epi32(__m128i _v) {
    int32_t v[4] __attribute__((aligned(16)));
    _mm_store_si128((__m128i*)v, _v);

    printf("{");
    unsigned int i;
#if 0
    for (i=0; i<4; i++) printf(" %17.8f", q16_fixed_to_float(v[i] >> q16_fracbits));
#else
    for (i=0; i<4; i++) printf(" %17d", v[i]);
#endif
    printf("}");
}
#endif

// internal methods
void dotprod_cccq16_execute_mmx(dotprod_cccq16 _q, cq16_t * _x, cq16_t * _y);
void dotprod_cccq16_execute_mmx4(dotprod_cccq16 _q, cq16_t * _x, cq16_t * _y);

// basic dot product
//  _h      :   coefficients array [size: 1 x _n]
//  _x      :   input array [size: 1 x _n]
//  _n      :   input lengths
//  _y      :   output dot product
void dotprod_cccq16_run(cq16_t *     _h,
                        cq16_t *     _x,
                        unsigned int _n,
                        cq16_t *     _y)
{
    // initialize accumulators (separate I/Q)
    q16_at ri = 0;
    q16_at rq = 0;

    unsigned int i;
    // straightforward method using four multiplications
    for (i=0; i<_n; i++) {
        // real component
        ri += (q16_at)(_h[i].real) * (q16_at)(_x[i].real) -
              (q16_at)(_h[i].imag) * (q16_at)(_x[i].imag);

        // imaginary component
        rq += (q16_at)(_h[i].real) * (q16_at)(_x[i].imag) +
              (q16_at)(_h[i].imag) * (q16_at)(_x[i].real);
    }

    // return result
    (*_y).real = (ri >> q16_fracbits);
    (*_y).imag = (rq >> q16_fracbits);
}

// basic dotproduct, unrolling loop
//  _h      :   coefficients array [size: 1 x _n]
//  _x      :   input array [size: 1 x _n]
//  _n      :   input lengths
//  _y      :   output dot product
void dotprod_cccq16_run4(cq16_t *     _h,
                         cq16_t *     _x,
                         unsigned int _n,
                         cq16_t *     _y)
{
    // initialize accumulator (separate I/Q)
    q16_at ri = 0;
    q16_at rq = 0;

    // t = 4*(floor(_n/4))
    unsigned int t=(_n>>2)<<2; 

    unsigned int i;
    // compute dotprod in groups of 4 using straightforward method using
    // four multiplications
    for (i=0; i<t; i+=4) {
        // real component
        ri += (q16_at)(_h[i  ].real) * (q16_at)(_x[i  ].real) -
              (q16_at)(_h[i  ].imag) * (q16_at)(_x[i  ].imag);
        ri += (q16_at)(_h[i+1].real) * (q16_at)(_x[i+1].real) -
              (q16_at)(_h[i+1].imag) * (q16_at)(_x[i+1].imag);
        ri += (q16_at)(_h[i+2].real) * (q16_at)(_x[i+2].real) -
              (q16_at)(_h[i+2].imag) * (q16_at)(_x[i+2].imag);
        ri += (q16_at)(_h[i+3].real) * (q16_at)(_x[i+3].real) -
              (q16_at)(_h[i+3].imag) * (q16_at)(_x[i+3].imag);

        // imaginary component
        rq += (q16_at)(_h[i  ].real) * (q16_at)(_x[i  ].imag) +
              (q16_at)(_h[i  ].imag) * (q16_at)(_x[i  ].real);
        rq += (q16_at)(_h[i+1].real) * (q16_at)(_x[i+1].imag) +
              (q16_at)(_h[i+1].imag) * (q16_at)(_x[i+1].real);
        rq += (q16_at)(_h[i+2].real) * (q16_at)(_x[i+2].imag) +
              (q16_at)(_h[i+2].imag) * (q16_at)(_x[i+2].real);
        rq += (q16_at)(_h[i+3].real) * (q16_at)(_x[i+3].imag) +
              (q16_at)(_h[i+3].imag) * (q16_at)(_x[i+3].real);
    }

    // clean up remaining
    for ( ; i<_n; i++) {
        // real component
        ri += (q16_at)(_h[i].real) * (q16_at)(_x[i].real) -
              (q16_at)(_h[i].imag) * (q16_at)(_x[i].imag);

        // imaginary component
        rq += (q16_at)(_h[i].real) * (q16_at)(_x[i].imag) +
              (q16_at)(_h[i].imag) * (q16_at)(_x[i].real);
    }

    // return result
    (*_y).real = (ri >> q16_fracbits);
    (*_y).imag = (rq >> q16_fracbits);
}



//
// structured MMX dot product
//

struct dotprod_cccq16_s {
    q16_t * hi;         // coefficients array (in-phase)
    q16_t * hq;         // coefficients array (quadrature)
    unsigned int n;     // length
};

dotprod_cccq16 dotprod_cccq16_create(cq16_t *     _h,
                                     unsigned int _n)
{
    dotprod_cccq16 q = (dotprod_cccq16)malloc(sizeof(struct dotprod_cccq16_s));
    q->n = _n;

    // allocate memory for coefficients, 16-byte aligned
    q->hi = (q16_t*) _mm_malloc( 2*q->n*sizeof(q16_t), 16);
    q->hq = (q16_t*) _mm_malloc( 2*q->n*sizeof(q16_t), 16);

    // set coefficients, repeated
    //  hi = { _h[0].real, _h[0].real, ... _h[n-1].real, _h[n-1].real }
    //  hq = { _h[0].imag, _h[0].imag, ... _h[n-1].imag, _h[n-1].imag }
    unsigned int i;
    for (i=0; i<q->n; i++) {
        q->hi[2*i+0] = _h[i].real;
        q->hi[2*i+1] = _h[i].real;

        q->hq[2*i+0] = _h[i].imag;
        q->hq[2*i+1] = _h[i].imag;
    }

    // return object
    return q;
}

// re-create the structured dotprod object
dotprod_cccq16 dotprod_cccq16_recreate(dotprod_cccq16 _dp,
                                       cq16_t *       _h,
                                       unsigned int   _n)
{
    // completely destroy and re-create dotprod object
    dotprod_cccq16_destroy(_dp);
    _dp = dotprod_cccq16_create(_h,_n);
    return _dp;
}


void dotprod_cccq16_destroy(dotprod_cccq16 _q)
{
    // free coefficients arrays
    _mm_free(_q->hi);
    _mm_free(_q->hq);

    // free main object memory
    free(_q);
}

void dotprod_cccq16_print(dotprod_cccq16 _q)
{
    printf("dotprod_cccq16:\n");
    unsigned int i;
    // print coefficients to screen, skipping odd entries (due
    // to repeated coefficients)
    for (i=0; i<_q->n; i++) {
        printf("%3u : %12.8f + j%12.8f\n",
                i,
                q16_fixed_to_float(_q->hi[2*i]),
                q16_fixed_to_float(_q->hq[2*i]));
    }
}

// 
void dotprod_cccq16_execute(dotprod_cccq16 _q,
                            cq16_t *       _x,
                            cq16_t *       _y)
{
    dotprod_cccq16_execute_mmx(_q, _x, _y);
    return;

    // switch based on size
    if (_q->n < 64) {
        dotprod_cccq16_execute_mmx(_q, _x, _y);
    } else {
        dotprod_cccq16_execute_mmx4(_q, _x, _y);
    }
}

// use MMX/SSE extensions
//  0 : (A + jB) * (I + jJ) = (AI - BJ) + j(AJ + BI)
//  1 : (C + jD) * (K + jL) = (CK - DL) + j(CL + DK)
//  2 : (E + jF) * (M + jN) = (EM - FN) + j(EN + FM)
//  3 : (G + jH) * (O + jP) = (GO - HP) + j(GP + HO)
//
//  v       :   [   A   B   C   D   E   F   G   H   ]   size: 8x16
//  hi      :   [   I   I   K   K   M   M   O   O   ]   size: 8x16
//  hq      :   [   J   J   L   L   N   N   P   P   ]   size: 8x16
//
//  ti_lo   :   [   AI      BI      CK      DK      ]   size: 4x32
//  ti_hi   :   [   EM      FM      GO      HO      ]   size: 4x32
//  tq_lo   :   [   AJ      BJ      CL      DL      ]   size: 4x32
//  tq_hi   :   [   EN      FN      GP      HP      ]   size: 4x32
//
//  sum0 = ti_lo + ti_hi
//  sum1 = tq_lo + tq_hi
//  sum0    :   [   AI+EM   BI+FM   CK+GO   DK+HO   ]   size: 4x32
//  sum1    :   [   AJ+EN   BJ+FN   CL+GP   DL+HP   ]   size: 4x32
void dotprod_cccq16_execute_mmx(dotprod_cccq16 _q,
                                cq16_t *       _x,
                                cq16_t *       _y)
{
    // type cast input as real array
    q16_t * x = (q16_t*) _x;
    
    // double effective length
    unsigned int n = 2*_q->n;

    // input, coefficients, multiply/accumulate vectors
    __m128i v;   // input vector
    __m128i hi;  // coefficients vector
    __m128i hq;  // coefficients vector
    __m128i ci_lo, ci_hi;   // output multiplication lo/hi (v * hi)
    __m128i cq_lo, cq_hi;   // output multiplication lo/hi (v * hq)
    __m128i ti_lo, ti_hi;   // unpacked lo/hi
    __m128i tq_lo, tq_hi;   // unpacked lo/hi
    __m128i sum0 = _mm_setzero_si128();
    __m128i sum1 = _mm_setzero_si128();

    __m128i acc0 = _mm_setzero_si128();
    __m128i acc1 = _mm_setzero_si128();


    // t = 8*(floor(_n/8))
    unsigned int t = (n >> 3) << 3;

    //
    unsigned int i;
    for (i=0; i<t; i+=8) {
        // load inputs into register (unaligned)
        // v: { x0.real, x0.imag, x1.real, x1.imag, x2.real, x2.imag, x3.real, x3.imag }
        v = _mm_loadu_si128( (__m128i*) (&x[i]) );

        // load coefficients into register (aligned)
        // hi: { h0.real, x0.imag, x1.real, x1.imag, x2.real, x2.imag, x3.real, x3.imag }
        hi = _mm_load_si128( (__m128i*) (&_q->hi[i]) );
        hq = _mm_load_si128( (__m128i*) (&_q->hq[i]) );

        // compute parallel multiplications:
        // multiply two 8x16-bit registers into two 4x32-bit registers
        ci_lo = _mm_mullo_epi16(v, hi);
        ci_hi = _mm_mulhi_epi16(v, hi);

        cq_lo = _mm_mullo_epi16(v, hq);
        cq_hi = _mm_mulhi_epi16(v, hq);

        // unpack bytes (re-align)
        ti_lo = _mm_unpacklo_epi16(ci_lo, ci_hi);
        ti_hi = _mm_unpackhi_epi16(ci_lo, ci_hi);
       
        tq_lo = _mm_unpacklo_epi16(cq_lo, cq_hi);
        tq_hi = _mm_unpackhi_epi16(cq_lo, cq_hi);

        // parallel addition
        sum0 = _mm_add_epi32(ti_lo, ti_hi);
        sum1 = _mm_add_epi32(tq_lo, tq_hi);

        acc0 = _mm_add_epi32(acc0, sum0);
        acc1 = _mm_add_epi32(acc1, sum1);
#if DEBUG_DOTPROD_CCCQ16_MMX
        printf("**i=%3u, t=%3u\n", i, t);
        printf("  v     :   "); _mm_printq16_epi16(v);      printf("\n");
        printf("  hi    :   "); _mm_printq16_epi16(hi);     printf("\n");
        printf("  hq    :   "); _mm_printq16_epi16(hq);     printf("\n");

        //printf("  ci_lo :   "); _mm_printq16_epi16(ci_lo);  printf("\n");
        //printf("  ci_hi :   "); _mm_printq16_epi16(ci_hi);  printf("\n");
        //printf("  cq_lo :   "); _mm_printq16_epi16(cq_lo);  printf("\n");
        //printf("  cq_hi :   "); _mm_printq16_epi16(cq_hi);  printf("\n");

        printf("  ti_lo :   "); _mm_printq16_epi32(ti_lo);  printf("\n");
        printf("  ti_hi :   "); _mm_printq16_epi32(ti_hi);  printf("\n");
        printf("  tq_lo :   "); _mm_printq16_epi32(tq_lo);  printf("\n");
        printf("  tq_hi :   "); _mm_printq16_epi32(tq_hi);  printf("\n");
#endif
    }

    // aligned output arrays: each a 4x32-bit register
    //q16_at w[4]  __attribute__((aligned(16)));
    q16_at w0[4] __attribute__((aligned(16)));
    q16_at w1[4] __attribute__((aligned(16)));
    
    // unpack...
    _mm_store_si128((__m128i*)w0, acc0);
    _mm_store_si128((__m128i*)w1, acc1);

    // in-phase   (AI+EM) + (CK+GO) - (BJ+FN) - (DL+HP)
    q16_at sum_i = w0[0]  +  w0[2]  -  w1[1]  -  w1[3];

    // quadrature (AJ+EN) + (CL+GP) + (BI+FM) + (DK+HO)
    q16_at sum_q = w1[0]  +  w1[2]  +  w0[1]  +  w0[3];

    // cleanup remaining computations
    // (a + jb)(c + jd) = (ac - bd) + j(ad + bc)
    //  x   :   [   A   B   ]
    //  hi  :   [   C   C   ]
    //  hq  :   [   D   D   ]
    for (i=t; i<n; i+=2) {
        //printf("cleanup: i=%3u, t=%3u, q->n=%3u, n=%3u\n", i, t, _q->n, n);
        sum_i += (q16_at)(_q->hi[i]) * (q16_at)(x[i+0]) -
                 (q16_at)(_q->hq[i]) * (q16_at)(x[i+1]);

        sum_q += (q16_at)(_q->hi[i]) * (q16_at)(x[i+1]) +
                 (q16_at)(_q->hq[i]) * (q16_at)(x[i+0]);
    }

    // set return value
    (*_y).real = (sum_i >> q16_fracbits);
    (*_y).imag = (sum_q >> q16_fracbits);
}

// use MMX/SSE extensions, unrolled loop
void dotprod_cccq16_execute_mmx4(dotprod_cccq16 _q,
                                 cq16_t *       _x,
                                 cq16_t *       _y)
{
    dotprod_cccq16_execute_mmx(_q, _x, _y);
}

