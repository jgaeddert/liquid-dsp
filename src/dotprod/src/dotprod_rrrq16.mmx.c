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

#define DEBUG_DOTPROD_RRRQ16_MMX   1

// internal methods
void dotprod_rrrq16_execute_mmx(dotprod_rrrq16 _q, q16_t * _x, q16_t * _y);
void dotprod_rrrq16_execute_mmx4(dotprod_rrrq16 _q, q16_t * _x, q16_t * _y);

// basic dot product
//  _h      :   coefficients array [size: 1 x _n]
//  _x      :   input array [size: 1 x _n]
//  _n      :   input lengths
//  _y      :   output dot product
void dotprod_rrrq16_run(q16_t * _h,
                        q16_t * _x,
                        unsigned int _n,
                        q16_t * _y)
{
    // initialize accumulator
    q16_at r=0;

    unsigned int i;
    for (i=0; i<_n; i++)
        r += (q16_at)_h[i] * (q16_at)_x[i];

    // return result
    *_y = (r >> q16_fracbits);
}

// basic dotproduct, unrolling loop
//  _h      :   coefficients array [size: 1 x _n]
//  _x      :   input array [size: 1 x _n]
//  _n      :   input lengths
//  _y      :   output dot product
void dotprod_rrrq16_run4(q16_t * _h,
                         q16_t * _x,
                         unsigned int _n,
                         q16_t * _y)
{
    // initialize accumulator
    q16_at r=0;

    // t = 4*(floor(_n/8))
    unsigned int t=(_n>>2)<<2; 

    // compute dotprod in groups of 4
    unsigned int i;
    for (i=0; i<t; i+=4) {
        r += (q16_at)_h[i  ] * (q16_at)_x[i  ];
        r += (q16_at)_h[i+1] * (q16_at)_x[i+1];
        r += (q16_at)_h[i+2] * (q16_at)_x[i+2];
        r += (q16_at)_h[i+3] * (q16_at)_x[i+3];
    }

    // clean up remaining
    for ( ; i<_n; i++)
        r += (q16_at)_h[i] * (q16_at)_x[i];

    // return result
    *_y = (r >> q16_fracbits);
}


//
// structured MMX dot product
//

struct dotprod_rrrq16_s {
    q16_t * h;          // coefficients array
    unsigned int n;     // length
};

dotprod_rrrq16 dotprod_rrrq16_create(q16_t * _h,
                                     unsigned int _n)
{
    dotprod_rrrq16 q = (dotprod_rrrq16)malloc(sizeof(struct dotprod_rrrq16_s));
    q->n = _n;

    // allocate memory for coefficients, 16-byte aligned
    q->h = (q16_t*) _mm_malloc( q->n*sizeof(q16_t), 16);

    // set coefficients
    memmove(q->h, _h, _n*sizeof(q16_t));

    // return object
    return q;
}

// re-create the structured dotprod object
dotprod_rrrq16 dotprod_rrrq16_recreate(dotprod_rrrq16 _dp,
                                       q16_t *        _h,
                                       unsigned int   _n)
{
    // completely destroy and re-create dotprod object
    dotprod_rrrq16_destroy(_dp);
    _dp = dotprod_rrrq16_create(_h,_n);
    return _dp;
}


void dotprod_rrrq16_destroy(dotprod_rrrq16 _q)
{
    _mm_free(_q->h);
    free(_q);
}

void dotprod_rrrq16_print(dotprod_rrrq16 _q)
{
    printf("dotprod_rrrq16:\n");
    unsigned int i;
    for (i=0; i<_q->n; i++)
        printf("%3u : %12.8f\n", i, q16_fixed_to_float(_q->h[i]));
}

// 
void dotprod_rrrq16_execute(dotprod_rrrq16 _q,
                            q16_t *        _x,
                            q16_t *        _y)
{
#if 0
    // switch based on size
    if (_q->n < 16) {
        dotprod_rrrq16_execute_mmx(_q, _x, _y);
    } else {
        dotprod_rrrq16_execute_mmx4(_q, _x, _y);
    }
#else
    dotprod_rrrq16_execute_mmx(_q, _x, _y);
#endif
}

// use MMX/SSE extensions
void dotprod_rrrq16_execute_mmx(dotprod_rrrq16 _q,
                                q16_t *        _x,
                                q16_t *        _y)
{
    // first cut: ...
    __m128i v;   // input vector
    __m128i h;   // coefficients vector
    __m128i s;   // dot product
    __m128i sum;
    sum = _mm_xor_si128(sum, sum); // load zeros into sum register
    
    // t = 8*(floor(_n/8))
    unsigned int t = (_q->n >> 3) << 3;

    //
    unsigned int i;
    for (i=0; i<t; i+=8) {
        // load inputs into register (unaligned)
        v = _mm_loadu_si128(&_x[i]);

        // load coefficients into register (aligned)
        h = _mm_loadu_si128(&_q->h[i]);

        // multiply and accumulate two 8x16-bit registers
        // into one 4x32-bit register
        s = _mm_madd_epi16(v, h);
       
        // parallel addition
        sum = _mm_add_epi32(sum, s);
    }

    // aligned output array: one 4x32-bit register
    q16_at w[4] __attribute__((aligned(16)));

#if HAVE_PMMINTRIN_H
    // TODO : check this
    // fold down into single value
    __m128i z;
    z   = _mm_xor_si128(z, z);      // set to zero
    sum = _mm_hadd_epi32(sum, z);
    sum = _mm_hadd_epi32(sum, z);
    sum = _mm_hadd_epi32(sum, z);
   
    // unload single (lower value)
    _mm_store_si128(w, sum);
    q16_at total = w[0];
#else
    // unload packed array
    _mm_store_si128(w, sum);
    q16_at total = w[0] + w[1] + w[2] + w[3];
#endif

    // cleanup
    for (; i<_q->n; i++)
        total += (q16_at)_x[i] * (q16_at)(_q->h[i]);

    // set return value, shifting appropriately
    *_y = (q16_t)(total >> q16_fracbits);
}

// use MMX/SSE extensions, unrolled loop
void dotprod_rrrq16_execute_mmx4(dotprod_rrrq16 _q,
                                 q16_t *        _x,
                                 q16_t *        _y)
{
}

