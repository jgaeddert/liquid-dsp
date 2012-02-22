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

#define DEBUG_DOTPROD_CRCQ16_MMX   0

// internal methods
void dotprod_crcq16_execute_mmx(dotprod_crcq16 _q, cq16_t * _x, cq16_t * _y);
void dotprod_crcq16_execute_mmx4(dotprod_crcq16 _q, cq16_t * _x, cq16_t * _y);

// basic dot product
//  _h      :   coefficients array [size: 1 x _n]
//  _x      :   input array [size: 1 x _n]
//  _n      :   input lengths
//  _y      :   output dot product
void dotprod_crcq16_run(q16_t *      _h,
                        cq16_t *     _x,
                        unsigned int _n,
                        cq16_t *     _y)
{
    // initialize accumulators (separate I/Q)
    q16_at ri = 0;
    q16_at rq = 0;

    unsigned int i;
    for (i=0; i<_n; i++) {
        ri += (q16_at)_h[i] * (q16_at)(_x[i].real);
        rq += (q16_at)_h[i] * (q16_at)(_x[i].imag);
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
void dotprod_crcq16_run4(q16_t *      _h,
                         cq16_t *     _x,
                         unsigned int _n,
                         cq16_t *     _y)
{
    // initialize accumulator (separate I/Q)
    q16_at ri = 0;
    q16_at rq = 0;

    // t = 4*(floor(_n/4))
    unsigned int t=(_n>>2)<<2; 

    // compute dotprod in groups of 4
    unsigned int i;
    for (i=0; i<t; i+=4) {
        // real component
        ri += (q16_at)_h[i  ] * (q16_at)(_x[i  ].real);
        ri += (q16_at)_h[i+1] * (q16_at)(_x[i+1].real);
        ri += (q16_at)_h[i+2] * (q16_at)(_x[i+2].real);
        ri += (q16_at)_h[i+3] * (q16_at)(_x[i+3].real);

        // imaginary component
        rq += (q16_at)_h[i  ] * (q16_at)(_x[i  ].imag);
        rq += (q16_at)_h[i+1] * (q16_at)(_x[i+1].imag);
        rq += (q16_at)_h[i+2] * (q16_at)(_x[i+2].imag);
        rq += (q16_at)_h[i+3] * (q16_at)(_x[i+3].imag);
    }

    // clean up remaining
    for ( ; i<_n; i++) {
        ri += (q16_at)_h[i] * (q16_at)(_x[i].real);
        rq += (q16_at)_h[i] * (q16_at)(_x[i].imag);
    }

    // return result
    (*_y).real = (ri >> q16_fracbits);
    (*_y).imag = (rq >> q16_fracbits);
}


//
// structured MMX dot product
//

struct dotprod_crcq16_s {
    q16_t * h;          // coefficients array
    unsigned int n;     // length
};

dotprod_crcq16 dotprod_crcq16_create(q16_t * _h,
                                     unsigned int _n)
{
    dotprod_crcq16 q = (dotprod_crcq16)malloc(sizeof(struct dotprod_crcq16_s));
    q->n = _n;

    // allocate memory for coefficients, 16-byte aligned
    q->h = (q16_t*) _mm_malloc( q->n*sizeof(q16_t), 16);

    // set coefficients
    memmove(q->h, _h, _n*sizeof(q16_t));

    // return object
    return q;
}

// re-create the structured dotprod object
dotprod_crcq16 dotprod_crcq16_recreate(dotprod_crcq16 _dp,
                                       q16_t *        _h,
                                       unsigned int   _n)
{
    // completely destroy and re-create dotprod object
    dotprod_crcq16_destroy(_dp);
    _dp = dotprod_crcq16_create(_h,_n);
    return _dp;
}


void dotprod_crcq16_destroy(dotprod_crcq16 _q)
{
    _mm_free(_q->h);
    free(_q);
}

void dotprod_crcq16_print(dotprod_crcq16 _q)
{
    printf("dotprod_crcq16:\n");
    unsigned int i;
    for (i=0; i<_q->n; i++)
        printf("%3u : %12.8f\n", i, q16_fixed_to_float(_q->h[i]));
}

// 
void dotprod_crcq16_execute(dotprod_crcq16 _q,
                            cq16_t *       _x,
                            cq16_t *       _y)
{
    // switch based on size
    if (_q->n < 64) {
        dotprod_crcq16_execute_mmx(_q, _x, _y);
    } else {
        dotprod_crcq16_execute_mmx4(_q, _x, _y);
    }
}

// use MMX/SSE extensions
void dotprod_crcq16_execute_mmx(dotprod_crcq16 _q,
                                cq16_t *       _x,
                                cq16_t *       _y)
{
    // use ordinal calculation temporarily
    dotprod_crcq16_run(_q->h, _x, _q->n, _y);
}

// use MMX/SSE extensions, unrolled loop
void dotprod_crcq16_execute_mmx4(dotprod_crcq16 _q,
                                 cq16_t *       _x,
                                 cq16_t *       _y)
{
    // use ordinal calculation temporarily
    dotprod_crcq16_run4(_q->h, _x, _q->n, _y);
}

