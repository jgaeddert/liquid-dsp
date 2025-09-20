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

#define DEBUG_DOTPROD_RRRQ32_MMX   0

// internal methods
int dotprod_rrrq32_execute_mmx(dotprod_rrrq32 _q, q32_t * _x, q32_t * _y);
int dotprod_rrrq32_execute_mmx4(dotprod_rrrq32 _q, q32_t * _x, q32_t * _y);

// basic dot product
//  _h      :   coefficients array [size: 1 x _n]
//  _x      :   input array [size: 1 x _n]
//  _n      :   input lengths
//  _y      :   output dot product
int dotprod_rrrq32_run(q32_t *      _h,
                       q32_t *      _x,
                       unsigned int _n,
                       q32_t *      _y)
{
    // initialize accumulator
    q32_at r=0;

    unsigned int i;
    for (i=0; i<_n; i++)
        r += (q32_at)_h[i] * (q32_at)_x[i];

    // return result
    *_y = (r >> q32_fracbits);
    return LIQUID_OK;
}

// basic dotproduct, unrolling loop
//  _h      :   coefficients array [size: 1 x _n]
//  _x      :   input array [size: 1 x _n]
//  _n      :   input lengths
//  _y      :   output dot product
int dotprod_rrrq32_run4(q32_t *      _h,
                        q32_t *      _x,
                        unsigned int _n,
                        q32_t *      _y)
{
    // initialize accumulator
    q32_at r=0;

    // t = 4*(floor(_n/8))
    unsigned int t=(_n>>2)<<2; 

    // compute dotprod in groups of 4
    unsigned int i;
    for (i=0; i<t; i+=4) {
        r += (q32_at)_h[i  ] * (q32_at)_x[i  ];
        r += (q32_at)_h[i+1] * (q32_at)_x[i+1];
        r += (q32_at)_h[i+2] * (q32_at)_x[i+2];
        r += (q32_at)_h[i+3] * (q32_at)_x[i+3];
    }

    // clean up remaining
    for ( ; i<_n; i++)
        r += (q32_at)_h[i] * (q32_at)_x[i];

    // return result
    *_y = (r >> q32_fracbits);
    return LIQUID_OK;
}


//
// structured MMX dot product
//

struct dotprod_rrrq32_s {
    q32_t * h;          // coefficients array
    unsigned int n;     // length
};

dotprod_rrrq32 dotprod_rrrq32_create(q32_t * _h,
                                     unsigned int _n)
{
    dotprod_rrrq32 q = (dotprod_rrrq32)malloc(sizeof(struct dotprod_rrrq32_s));
    q->n = _n;

    // allocate memory for coefficients, 16-byte aligned
    q->h = (q32_t*) _mm_malloc( q->n*sizeof(q32_t), 16);

    // set coefficients
    memmove(q->h, _h, _n*sizeof(q32_t));

    // return object
    return q;
}

// create object with reversed coefficients
dotprod_rrrq32 dotprod_rrrq32_create_rev(q32_t *      _h,
                                         unsigned int _n)
{
    // copy coefficients to temporary buffer in reversed order
    q32_t h_rev[_n];
    unsigned int i;
    for (i=0; i<_n; i++)
        h_rev[i] = _h[_n-i-1];

    // create new object with reversed coefficients
    return dotprod_rrrq32_create(h_rev, _n);
}

// re-create the structured dotprod object
dotprod_rrrq32 dotprod_rrrq32_recreate(dotprod_rrrq32 _dp,
                                       q32_t *        _h,
                                       unsigned int   _n)
{
    // completely destroy and re-create dotprod object
    dotprod_rrrq32_destroy(_dp);
    _dp = dotprod_rrrq32_create(_h,_n);
    return _dp;
}

// copy object
dotprod_rrrq32 dotprod_rrrq32_copy(dotprod_rrrq32 q_orig)
{
    // validate input
    if (q_orig == NULL)
        return liquid_error_config("dotprod_rrrq32_copy().mmx, object cannot be NULL");

    dotprod_rrrq32 q_copy = (dotprod_rrrq32)malloc(sizeof(struct dotprod_rrrq32_s));
    q_copy->n = q_orig->n;

    // allocate memory for coefficients, 32-byte aligned
    q_copy->h = (q32_t*) _mm_malloc( q_copy->n*sizeof(q32_t), 32 );

    // copy coefficients array
    //  h = { _h[0], _h[1], _h[2], ... _h[n-1] }
    memmove(q_copy->h, q_orig->h, q_orig->n*sizeof(q32_t));

    // return object
    return q_copy;
}


int dotprod_rrrq32_destroy(dotprod_rrrq32 _q)
{
    _mm_free(_q->h);
    free(_q);
    return LIQUID_OK;
}

int dotprod_rrrq32_print(dotprod_rrrq32 _q)
{
    printf("dotprod_rrrq32:\n");
    unsigned int i;
    for (i=0; i<_q->n; i++)
        printf("%3u : %12.8f\n", i, q32_fixed_to_float(_q->h[i]));
    return LIQUID_OK;
}

// 
int dotprod_rrrq32_execute(dotprod_rrrq32 _q,
                           q32_t *        _x,
                           q32_t *        _y)
{
    // switch based on size
    if (_q->n < 64) {
        return dotprod_rrrq32_execute_mmx(_q, _x, _y);
    }
    return dotprod_rrrq32_execute_mmx4(_q, _x, _y);
}

// use MMX/SSE extensions
int dotprod_rrrq32_execute_mmx(dotprod_rrrq32 _q,
                               q32_t *        _x,
                               q32_t *        _y)
{
    return dotprod_rrrq32_run4(_q->h, _x, _q->n, _y);
}

// use MMX/SSE extensions, unrolled loop
int dotprod_rrrq32_execute_mmx4(dotprod_rrrq32 _q,
                                q32_t *        _x,
                                q32_t *        _y)
{
    return dotprod_rrrq32_execute_mmx(_q, _x, _y);
}

