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
// Fixed-point dot product
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "liquid.internal.h"

// portable structured dot product object
struct dotprod_cccq16_s
{
    cq16_t *     h;     // coefficients array
    unsigned int n;     // length
};

// basic dot product
//  _h      :   coefficients array [size: 1 x _n]
//  _x      :   input array [size: 1 x _n]
//  _n      :   input lengths
//  _y      :   output dot product
int dotprod_cccq16_run(cq16_t *     _h,
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
    return LIQUID_OK;
}

// basic dotproduct, unrolling loop
//  _h      :   coefficients array [size: 1 x _n]
//  _x      :   input array [size: 1 x _n]
//  _n      :   input lengths
//  _y      :   output dot product
int dotprod_cccq16_run4(cq16_t *     _h,
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
    return LIQUID_OK;
}

//
// structured dot product
//

// create structured dot product object
//  _h      :   coefficients array [size: 1 x _n]
//  _n      :   dot product length
dotprod_cccq16 dotprod_cccq16_create(cq16_t *     _h,
                                     unsigned int _n)
{
    dotprod_cccq16 q = (dotprod_cccq16) malloc(sizeof(struct dotprod_cccq16_s));
    q->n = _n;

    // allocate memory for coefficients (complex)
    q->h = (cq16_t*) malloc((q->n)*sizeof(cq16_t));

    // move coefficients
    memmove(q->h, _h, (q->n)*sizeof(cq16_t));

    // return object
    return q;
}

// create dot product object with coefficients in reverse order
//  _h      :   coefficients array [size: 1 x _n]
//  _n      :   dot product length
dotprod_cccq16 dotprod_cccq16_create_rev(cq16_t *       _h,
                                         unsigned int   _n)
{
    dotprod_cccq16 q = (dotprod_cccq16) malloc(sizeof(struct dotprod_cccq16_s));
    q->n = _n;

    // allocate memory for coefficients (complex)
    q->h = (cq16_t*) malloc((q->n)*sizeof(cq16_t));

    // copy coefficients in time-reversed order
    unsigned int i;
    for (i=0; i<_n; i++)
        q->h[i] = _h[_n-i-1];

    // return object
    return q;
}

// re-create dot product object
//  _q      :   old dot dot product object
//  _h      :   new coefficients [size: 1 x _n]
//  _n      :   new dot product size
dotprod_cccq16 dotprod_cccq16_recreate(dotprod_cccq16 _q,
                                       cq16_t *       _h,
                                       unsigned int   _n)
{
    // check to see if length has changed
    if (_q->n != _n) {
        // set new length
        _q->n = _n;

        // re-allocate memory
        _q->h = (cq16_t*) realloc(_q->h, (_q->n)*sizeof(cq16_t));
    }

    // move new coefficients
    memmove(_q->h, _h, (_q->n)*sizeof(cq16_t));

    // return re-structured object
    return _q;
}

// re-create dot product object with coefficients in reverse order
//  _q      :   old dot dot product object
//  _h      :   time-reversed new coefficients [size: 1 x _n]
//  _n      :   new dot product size
dotprod_cccq16 dotprod_cccq16_recreate_rev(dotprod_cccq16 _q,
                                           cq16_t *       _h,
                                           unsigned int   _n)
{
    // check to see if length has changed
    if (_q->n != _n) {
        // set new length
        _q->n = _n;

        // re-allocate memory
        _q->h = (cq16_t*) realloc(_q->h, (_q->n)*sizeof(cq16_t));
    }

    // copy coefficients in time-reversed order
    unsigned int i;
    for (i=0; i<_n; i++)
        _q->h[i] = _h[_n-i-1];

    // return re-structured object
    return _q;
}

// copy object
dotprod_cccq16 dotprod_cccq16_copy(dotprod_cccq16 q_orig)
{
    // validate input
    if (q_orig == NULL)
        return liquid_error_config("dotprod_%s_copy(), input object cannot be NULL", "xxxt");

    // create new base object and copy parameters
    dotprod_cccq16 q_copy = (dotprod_cccq16) malloc(sizeof(struct dotprod_cccq16_s));
    q_copy->n = q_orig->n;

    // allocate memory and copy coefficients
    q_copy->h = (cq16_t*) malloc((q_copy->n)*sizeof(cq16_t));
    memmove(q_copy->h, q_orig->h, (q_copy->n)*sizeof(cq16_t));

    // return new object
    return q_copy;
}

// destroy dot product object
int dotprod_cccq16_destroy(dotprod_cccq16 _q)
{
    free(_q->h);    // free coefficients memory
    free(_q);       // free main object memory
    return LIQUID_OK;
}

// print dot product object
int dotprod_cccq16_print(dotprod_cccq16 _q)
{
    printf("dotprod [%u elements]:\n", _q->n);
    unsigned int i;
    for (i=0; i<_q->n; i++) {
        printf("  %4u: %12.8f + j%12.8f\n", i, q16_fixed_to_float(_q->h[i].real),
                                               q16_fixed_to_float(_q->h[i].imag));
    }
    return LIQUID_OK;
}

// execute structured dot product
//  _q      :   dot product object
//  _x      :   input array [size: 1 x _n]
//  _y      :   output dot product
int dotprod_cccq16_execute(dotprod_cccq16 _q,
                           cq16_t *       _x,
                           cq16_t *       _y)
{
    // run basic dot product with unrolled loops
    dotprod_cccq16_run4(_q->h, _x, _q->n, _y);
    return LIQUID_OK;
}

