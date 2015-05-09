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

#define DEBUG_DOTPROD_CCCq32_MMX   0

#if DEBUG_DOTPROD_CCCq32_MMX
void _mm_printq32_epi16(__m128i _v);
void _mm_printq32_epi32(__m128i _v);
#endif

// internal methods
void dotprod_cccq32_execute_mmx(dotprod_cccq32 _q, cq32_t * _x, cq32_t * _y);
void dotprod_cccq32_execute_mmx4(dotprod_cccq32 _q, cq32_t * _x, cq32_t * _y);

// basic dot product
//  _h      :   coefficients array [size: 1 x _n]
//  _x      :   input array [size: 1 x _n]
//  _n      :   input lengths
//  _y      :   output dot product
void dotprod_cccq32_run(cq32_t *     _h,
                        cq32_t *     _x,
                        unsigned int _n,
                        cq32_t *     _y)
{
    // initialize accumulators (separate I/Q)
    q32_at ri = 0;
    q32_at rq = 0;

    unsigned int i;
    // straightforward method using four multiplications
    for (i=0; i<_n; i++) {
        // real component
        ri += (q32_at)(_h[i].real) * (q32_at)(_x[i].real) -
              (q32_at)(_h[i].imag) * (q32_at)(_x[i].imag);

        // imaginary component
        rq += (q32_at)(_h[i].real) * (q32_at)(_x[i].imag) +
              (q32_at)(_h[i].imag) * (q32_at)(_x[i].real);
    }

    // return result
    (*_y).real = (ri >> q32_fracbits);
    (*_y).imag = (rq >> q32_fracbits);
}

// basic dotproduct, unrolling loop
//  _h      :   coefficients array [size: 1 x _n]
//  _x      :   input array [size: 1 x _n]
//  _n      :   input lengths
//  _y      :   output dot product
void dotprod_cccq32_run4(cq32_t *     _h,
                         cq32_t *     _x,
                         unsigned int _n,
                         cq32_t *     _y)
{
    // initialize accumulator (separate I/Q)
    q32_at ri = 0;
    q32_at rq = 0;

    // t = 4*(floor(_n/4))
    unsigned int t=(_n>>2)<<2; 

    unsigned int i;
    // compute dotprod in groups of 4 using straightforward method using
    // four multiplications
    for (i=0; i<t; i+=4) {
        // real component
        ri += (q32_at)(_h[i  ].real) * (q32_at)(_x[i  ].real) -
              (q32_at)(_h[i  ].imag) * (q32_at)(_x[i  ].imag);
        ri += (q32_at)(_h[i+1].real) * (q32_at)(_x[i+1].real) -
              (q32_at)(_h[i+1].imag) * (q32_at)(_x[i+1].imag);
        ri += (q32_at)(_h[i+2].real) * (q32_at)(_x[i+2].real) -
              (q32_at)(_h[i+2].imag) * (q32_at)(_x[i+2].imag);
        ri += (q32_at)(_h[i+3].real) * (q32_at)(_x[i+3].real) -
              (q32_at)(_h[i+3].imag) * (q32_at)(_x[i+3].imag);

        // imaginary component
        rq += (q32_at)(_h[i  ].real) * (q32_at)(_x[i  ].imag) +
              (q32_at)(_h[i  ].imag) * (q32_at)(_x[i  ].real);
        rq += (q32_at)(_h[i+1].real) * (q32_at)(_x[i+1].imag) +
              (q32_at)(_h[i+1].imag) * (q32_at)(_x[i+1].real);
        rq += (q32_at)(_h[i+2].real) * (q32_at)(_x[i+2].imag) +
              (q32_at)(_h[i+2].imag) * (q32_at)(_x[i+2].real);
        rq += (q32_at)(_h[i+3].real) * (q32_at)(_x[i+3].imag) +
              (q32_at)(_h[i+3].imag) * (q32_at)(_x[i+3].real);
    }

    // clean up remaining
    for ( ; i<_n; i++) {
        // real component
        ri += (q32_at)(_h[i].real) * (q32_at)(_x[i].real) -
              (q32_at)(_h[i].imag) * (q32_at)(_x[i].imag);

        // imaginary component
        rq += (q32_at)(_h[i].real) * (q32_at)(_x[i].imag) +
              (q32_at)(_h[i].imag) * (q32_at)(_x[i].real);
    }

    // return result
    (*_y).real = (ri >> q32_fracbits);
    (*_y).imag = (rq >> q32_fracbits);
}



//
// structured MMX dot product
//

struct dotprod_cccq32_s {
    cq32_t * h;         // coefficients array (in-phase)
    unsigned int n;     // length
};

dotprod_cccq32 dotprod_cccq32_create(cq32_t *     _h,
                                     unsigned int _n)
{
    dotprod_cccq32 q = (dotprod_cccq32)malloc(sizeof(struct dotprod_cccq32_s));
    q->n = _n;

    // allocate memory for coefficients, 16-byte aligned
    q->h = (cq32_t*) malloc( q->n*sizeof(cq32_t) );

    // set coefficients
    memmove(q->h, _h, q->n*sizeof(cq32_t));

    // return object
    return q;
}

// re-create the structured dotprod object
dotprod_cccq32 dotprod_cccq32_recreate(dotprod_cccq32 _dp,
                                       cq32_t *       _h,
                                       unsigned int   _n)
{
    // completely destroy and re-create dotprod object
    dotprod_cccq32_destroy(_dp);
    _dp = dotprod_cccq32_create(_h,_n);
    return _dp;
}


void dotprod_cccq32_destroy(dotprod_cccq32 _q)
{
    // free coefficients array
    free(_q->h);

    // free main object memory
    free(_q);
}

void dotprod_cccq32_print(dotprod_cccq32 _q)
{
    printf("dotprod_cccq32:\n");
    unsigned int i;
    // print coefficients to screen, skipping odd entries (due
    // to repeated coefficients)
    for (i=0; i<_q->n; i++) {
        printf("%3u : %12.8f + j%12.8f\n",
                i,
                q32_fixed_to_float(_q->h[i].real),
                q32_fixed_to_float(_q->h[i].imag));
    }
}

// 
void dotprod_cccq32_execute(dotprod_cccq32 _q,
                            cq32_t *       _x,
                            cq32_t *       _y)
{
    dotprod_cccq32_execute_mmx(_q, _x, _y);
}

// use MMX/SSE extensions
void dotprod_cccq32_execute_mmx(dotprod_cccq32 _q,
                                cq32_t *       _x,
                                cq32_t *       _y)
{
    dotprod_cccq32_run4(_q->h, _x, _q->n, _y);
}

// use MMX/SSE extensions, unrolled loop
void dotprod_cccq32_execute_mmx4(dotprod_cccq32 _q,
                                 cq32_t *       _x,
                                 cq32_t *       _y)
{
    dotprod_cccq32_run4(_q->h, _x, _q->n, _y);
}

