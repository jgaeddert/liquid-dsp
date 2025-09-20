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
#include <immintrin.h>
#include <assert.h>

#include "liquid.internal.h"

#define DEBUG_DOTPROD_CCCQ32_MMX   0

#if DEBUG_DOTPROD_CCCQ32_MMX
int _mm_printq32_epi16(__m128i _v);
int _mm_printq32_epi32(__m128i _v);
#endif

// internal methods
int dotprod_cccq32_execute_mmx(dotprod_cccq32 _q, cq32_t * _x, cq32_t * _y);
int dotprod_cccq32_execute_mmx4(dotprod_cccq32 _q, cq32_t * _x, cq32_t * _y);

// basic dot product
//  _h      :   coefficients array [size: 1 x _n]
//  _x      :   input array [size: 1 x _n]
//  _n      :   input lengths
//  _y      :   output dot product
int dotprod_cccq32_run(cq32_t *     _h,
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
    return LIQUID_OK;
}

// basic dotproduct, unrolling loop
//  _h      :   coefficients array [size: 1 x _n]
//  _x      :   input array [size: 1 x _n]
//  _n      :   input lengths
//  _y      :   output dot product
int dotprod_cccq32_run4(cq32_t *     _h,
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
    return LIQUID_OK;
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

// create object with reversed coefficients
dotprod_cccq32 dotprod_cccq32_create_rev(cq32_t *     _h,
                                         unsigned int _n)
{
    // copy coefficients to temporary buffer in reversed order
    cq32_t h_rev[_n];
    unsigned int i;
    for (i=0; i<_n; i++)
        h_rev[i] = _h[_n-i-1];

    // create new object with reversed coefficients
    return dotprod_cccq32_create(h_rev, _n);
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

// copy object
dotprod_cccq32 dotprod_cccq32_copy(dotprod_cccq32 q_orig)
{
    // validate input
    if (q_orig == NULL)
        return liquid_error_config("dotprod_cccq32_copy().mmx, object cannot be NULL");

    dotprod_cccq32 q_copy = (dotprod_cccq32)malloc(sizeof(struct dotprod_cccq32_s));
    q_copy->n = q_orig->n;

    // allocate memory for coefficients, 32-byte aligned (repeated)
    q_copy->h = (cq32_t*) _mm_malloc( q_copy->n*sizeof(cq32_t), 32 );

    // copy coefficients array
    memmove(q_copy->h, q_orig->h, 2*q_orig->n*sizeof(cq32_t));

    // return object
    return q_copy;
}


int dotprod_cccq32_destroy(dotprod_cccq32 _q)
{
    // free coefficients array
    free(_q->h);

    // free main object memory
    free(_q);
    return LIQUID_OK;
}

int dotprod_cccq32_print(dotprod_cccq32 _q)
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
    return LIQUID_OK;
}

// 
int dotprod_cccq32_execute(dotprod_cccq32 _q,
                           cq32_t *       _x,
                           cq32_t *       _y)
{
    return dotprod_cccq32_execute_mmx(_q, _x, _y);
}

// use MMX/SSE extensions
int dotprod_cccq32_execute_mmx(dotprod_cccq32 _q,
                               cq32_t *       _x,
                               cq32_t *       _y)
{
    return dotprod_cccq32_run4(_q->h, _x, _q->n, _y);
}

// use MMX/SSE extensions, unrolled loop
int dotprod_cccq32_execute_mmx4(dotprod_cccq32 _q,
                                cq32_t *       _x,
                                cq32_t *       _y)
{
    return dotprod_cccq32_run4(_q->h, _x, _q->n, _y);
}

