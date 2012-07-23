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

#define DEBUG_DOTPROD_CRCq32_MMX   0

#if DEBUG_DOTPROD_CRCq32_MMX
void _mm_printq32_epi16(__m128i _v);
void _mm_printq32_epi32(__m128i _v);
#endif

// internal methods
void dotprod_crcq32_execute_mmx(dotprod_crcq32 _q, cq32_t * _x, cq32_t * _y);
void dotprod_crcq32_execute_mmx4(dotprod_crcq32 _q, cq32_t * _x, cq32_t * _y);

// alternate methods
void dotprod_crcq32_execute_mmx_packed(dotprod_crcq32 _q, cq32_t * _x, cq32_t * _y);

// basic dot product
//  _h      :   coefficients array [size: 1 x _n]
//  _x      :   input array [size: 1 x _n]
//  _n      :   input lengths
//  _y      :   output dot product
void dotprod_crcq32_run(q32_t *      _h,
                        cq32_t *     _x,
                        unsigned int _n,
                        cq32_t *     _y)
{
    // initialize accumulators (separate I/Q)
    q32_at ri = 0;
    q32_at rq = 0;

    unsigned int i;
    for (i=0; i<_n; i++) {
        ri += (q32_at)_h[i] * (q32_at)(_x[i].real);
        rq += (q32_at)_h[i] * (q32_at)(_x[i].imag);
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
void dotprod_crcq32_run4(q32_t *      _h,
                         cq32_t *     _x,
                         unsigned int _n,
                         cq32_t *     _y)
{
    // initialize accumulator (separate I/Q)
    q32_at ri = 0;
    q32_at rq = 0;

    // t = 4*(floor(_n/4))
    unsigned int t=(_n>>2)<<2; 

    // compute dotprod in groups of 4
    unsigned int i;
    for (i=0; i<t; i+=4) {
        // real component
        ri += (q32_at)_h[i  ] * (q32_at)(_x[i  ].real);
        ri += (q32_at)_h[i+1] * (q32_at)(_x[i+1].real);
        ri += (q32_at)_h[i+2] * (q32_at)(_x[i+2].real);
        ri += (q32_at)_h[i+3] * (q32_at)(_x[i+3].real);

        // imaginary component
        rq += (q32_at)_h[i  ] * (q32_at)(_x[i  ].imag);
        rq += (q32_at)_h[i+1] * (q32_at)(_x[i+1].imag);
        rq += (q32_at)_h[i+2] * (q32_at)(_x[i+2].imag);
        rq += (q32_at)_h[i+3] * (q32_at)(_x[i+3].imag);
    }

    // clean up remaining
    for ( ; i<_n; i++) {
        ri += (q32_at)_h[i] * (q32_at)(_x[i].real);
        rq += (q32_at)_h[i] * (q32_at)(_x[i].imag);
    }

    // return result
    (*_y).real = (ri >> q32_fracbits);
    (*_y).imag = (rq >> q32_fracbits);
}


//
// structured MMX dot product
//

struct dotprod_crcq32_s {
    q32_t * h;          // coefficients array
    unsigned int n;     // length
};

dotprod_crcq32 dotprod_crcq32_create(q32_t *      _h,
                                     unsigned int _n)
{
    dotprod_crcq32 q = (dotprod_crcq32)malloc(sizeof(struct dotprod_crcq32_s));
    q->n = _n;

    // allocate memory for coefficients
    q->h = (q32_t*) malloc( q->n*sizeof(q32_t) );

    // set coefficients, repeated
    memmove(q->h, _h, q->n*sizeof(q32_t));

    // return object
    return q;
}

// re-create the structured dotprod object
dotprod_crcq32 dotprod_crcq32_recreate(dotprod_crcq32 _dp,
                                       q32_t *        _h,
                                       unsigned int   _n)
{
    // completely destroy and re-create dotprod object
    dotprod_crcq32_destroy(_dp);
    _dp = dotprod_crcq32_create(_h,_n);
    return _dp;
}


void dotprod_crcq32_destroy(dotprod_crcq32 _q)
{
    free(_q->h);
    free(_q);
}

void dotprod_crcq32_print(dotprod_crcq32 _q)
{
    printf("dotprod_crcq32:\n");
    unsigned int i;
    // print coefficients to screen
    for (i=0; i<_q->n; i++)
        printf("%3u : %12.8f\n", i, q32_fixed_to_float(_q->h[i]));
}

// 
void dotprod_crcq32_execute(dotprod_crcq32 _q,
                            cq32_t *       _x,
                            cq32_t *       _y)
{
    // switch based on size
    if (_q->n < 64) {
        dotprod_crcq32_execute_mmx(_q, _x, _y);
    } else {
        dotprod_crcq32_execute_mmx4(_q, _x, _y);
    }
}

// use MMX/SSE extensions
void dotprod_crcq32_execute_mmx(dotprod_crcq32 _q,
                                cq32_t *       _x,
                                cq32_t *       _y)
{
    dotprod_crcq32_run4(_q->h, _x, _q->n, _y);
}

// use MMX/SSE extensions, unrolled loop
void dotprod_crcq32_execute_mmx4(dotprod_crcq32 _q,
                                 cq32_t *       _x,
                                 cq32_t *       _y)
{
    dotprod_crcq32_run4(_q->h, _x, _q->n, _y);
}

