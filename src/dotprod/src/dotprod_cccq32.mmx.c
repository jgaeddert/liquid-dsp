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

