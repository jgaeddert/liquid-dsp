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
// Generic dot product
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// portable structured dot product object
struct DOTPROD(_s) {
    TC * h;             // coefficients array
    unsigned int n;     // length
};

// basic dot product
//  _h      :   coefficients array [size: 1 x _n]
//  _x      :   input array [size: 1 x _n]
//  _n      :   input lengths
//  _y      :   output dot product
void DOTPROD(_run)(TC *         _h,
                   TI *         _x,
                   unsigned int _n,
                   TO *         _y)
{
    // initialize accumulator
    TO r=0;

    unsigned int i;
    for (i=0; i<_n; i++)
        r += _h[i] * _x[i];

    // return result
    *_y = r;
}

// basic dotproduct, unrolling loop
//  _h      :   coefficients array [size: 1 x _n]
//  _x      :   input array [size: 1 x _n]
//  _n      :   input lengths
//  _y      :   output dot product
void DOTPROD(_run4)(TC *         _h,
                    TI *         _x,
                    unsigned int _n,
                    TO *         _y)
{
    // initialize accumulator
    TO r=0;

    // t = 4*(floor(_n/4))
    unsigned int t=(_n>>2)<<2; 

    // compute dotprod in groups of 4
    unsigned int i;
    for (i=0; i<t; i+=4) {
        r += _h[i]   * _x[i];
        r += _h[i+1] * _x[i+1];
        r += _h[i+2] * _x[i+2];
        r += _h[i+3] * _x[i+3];
    }

    // clean up remaining
    for ( ; i<_n; i++)
        r += _h[i] * _x[i];

    // return result
    *_y = r;
}

//
// structured dot product
//

// create structured dot product object
//  _h      :   coefficients array [size: 1 x _n]
//  _n      :   dot product length
DOTPROD() DOTPROD(_create)(TC *         _h,
                           unsigned int _n)
{
    DOTPROD() q = (DOTPROD()) malloc(sizeof(struct DOTPROD(_s)));
    q->n = _n;

    // allocate memory for coefficients
    q->h = (TC*) malloc((q->n)*sizeof(TC));

    // move coefficients
    memmove(q->h, _h, (q->n)*sizeof(TC));

    // return object
    return q;
}

// re-create dot product object
//  _q      :   old dot dot product object
//  _h      :   new coefficients [size: 1 x _n]
//  _n      :   new dot product size
DOTPROD() DOTPROD(_recreate)(DOTPROD()    _q,
                             TC *         _h,
                             unsigned int _n)
{
    // check to see if length has changed
    if (_q->n != _n) {
        // set new length
        _q->n = _n;

        // re-allocate memory
        _q->h = (TC*) realloc(_q->h, (_q->n)*sizeof(TC));
    }

    // move new coefficients
    memmove(_q->h, _h, (_q->n)*sizeof(TC));

    // return re-structured object
    return _q;
}

// destroy dot product object
void DOTPROD(_destroy)(DOTPROD() _q)
{
    free(_q->h);    // free coefficients memory
    free(_q);       // free main object memory
}

// print dot product object
void DOTPROD(_print)(DOTPROD() _q)
{
    printf("dotprod [portable, %u coefficients]:\n", _q->n);
    unsigned int i;
    for (i=0; i<_q->n; i++) {
        printf("  %4u: %12.8f + j*%12.8f\n", i,
                                             crealf(_q->h[i]),
                                             cimagf(_q->h[i]));
    }
}

// execute structured dot product
//  _q      :   dot product object
//  _x      :   input array [size: 1 x _n]
//  _y      :   output dot product
void DOTPROD(_execute)(DOTPROD() _q,
                       TI *      _x,
                       TO *      _y)
{
    // run basic dot product with unrolled loops
    DOTPROD(_run4)(_q->h, _x, _q->n, _y);
}

