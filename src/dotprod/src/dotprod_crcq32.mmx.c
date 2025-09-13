/*
 * Copyright (c) 2007 - 2025 Joseph Gaeddert
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

#define DEBUG_DOTPROD_CRCq32_MMX   0

#if DEBUG_DOTPROD_CRCq32_MMX
int _mm_printq32_epi16(__m128i _v);
int _mm_printq32_epi32(__m128i _v);
#endif

// internal methods
int dotprod_crcq32_execute_mmx(dotprod_crcq32 _q, cq32_t * _x, cq32_t * _y);
int dotprod_crcq32_execute_mmx4(dotprod_crcq32 _q, cq32_t * _x, cq32_t * _y);

// alternate methods
int dotprod_crcq32_execute_mmx_packed(dotprod_crcq32 _q, cq32_t * _x, cq32_t * _y);

// basic dot product
//  _h      :   coefficients array [size: 1 x _n]
//  _x      :   input array [size: 1 x _n]
//  _n      :   input lengths
//  _y      :   output dot product
int dotprod_crcq32_run(q32_t *      _h,
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
    return LIQUID_OK;
}

// basic dotproduct, unrolling loop
//  _h      :   coefficients array [size: 1 x _n]
//  _x      :   input array [size: 1 x _n]
//  _n      :   input lengths
//  _y      :   output dot product
int dotprod_crcq32_run4(q32_t *      _h,
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
    return LIQUID_OK;
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


int dotprod_crcq32_destroy(dotprod_crcq32 _q)
{
    free(_q->h);
    free(_q);
    return LIQUID_OK;
}

int dotprod_crcq32_print(dotprod_crcq32 _q)
{
    printf("dotprod_crcq32:\n");
    unsigned int i;
    // print coefficients to screen
    for (i=0; i<_q->n; i++)
        printf("%3u : %12.8f\n", i, q32_fixed_to_float(_q->h[i]));
    return LIQUID_OK;
}

// 
int dotprod_crcq32_execute(dotprod_crcq32 _q,
                           cq32_t *       _x,
                           cq32_t *       _y)
{
    // switch based on size
    if (_q->n < 64) {
        return dotprod_crcq32_execute_mmx(_q, _x, _y);
    }

    return dotprod_crcq32_execute_mmx4(_q, _x, _y);
}

// use MMX/SSE extensions
int dotprod_crcq32_execute_mmx(dotprod_crcq32 _q,
                               cq32_t *       _x,
                               cq32_t *       _y)
{
    return dotprod_crcq32_run4(_q->h, _x, _q->n, _y);
}

// use MMX/SSE extensions, unrolled loop
int dotprod_crcq32_execute_mmx4(dotprod_crcq32 _q,
                                cq32_t *       _x,
                                cq32_t *       _y)
{
    return dotprod_crcq32_run4(_q->h, _x, _q->n, _y);
}

