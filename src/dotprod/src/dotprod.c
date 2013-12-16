/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011, 2012, 2013 Joseph Gaeddert
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
// Generic dot product
//
#ifdef __ARM_NEON__
#include <arm_neon.h>
extern float get_sum(float32x4_t);
#endif
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

// basic dotproduct, NEON intrinsics and NEON function
//  _h      :   coefficients array [size: 1 x _n]
//  _x      :   input array [size: 1 x _n]
//  _n      :   input lengths
//  _y      :   output dot product
void DOTPROD(_NEON)(TC *_h,
                    TI *_x,
                    unsigned int _n,
                    TO * _y)
{
    // initialize accumulator
    TO r=0;

    // t = 4*(floor(_n/4))
    unsigned int t=(_n>>2)<<2;
                                                              	            	
    // compute dotprod in groups of 4
    unsigned int i;
#ifdef __ARM_NEON__
    float32x4_t h_vec, x_vec, result_vec;
    result_vec = vdupq_n_f32(0);
    for(i=0; i<t; i+=4){
        h_vec = vld1q_f32(&_h[i]);
        x_vec = vld1q_f32(&_x[i]);
        result_vec = vmlaq_f32(result_vec, h_vec, x_vec);
    }
    r = get_sum(result_vec);
#else
    for (i=0; i<t; i+=4) {
        r += _h[i]   * _x[i];
        r += _h[i+1] * _x[i+1];
        r += _h[i+2] * _x[i+2];
        r += _h[i+3] * _x[i+3];
    }
#endif
    //clean up remainig
    for (i=t ; i<_n; i++)
        r += _h[i] * _x[i];

   // return result
   *_y = r;
}

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
// execute structured dot product with NEON Intrinsics
//  _q      :   dot product object
//  _x      :   input array [size: 1 x _n]
//  _y      :   output dot product
void DOTPROD(_executeNEON)(DOTPROD() _q,
                       TI * _x,
                       TO * _y)
{ // run dot product with NEON intrinsics
    DOTPROD(_NEON)(_q->h, _x, _q->n, _y);
}

