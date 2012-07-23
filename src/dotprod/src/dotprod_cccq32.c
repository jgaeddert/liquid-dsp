/*
 * Copyright (c) 2012 Joseph Gaeddert
 * Copyright (c) 2012 Virginia Polytechnic Institute & State University
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
// Fixed-point dot product
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "liquid.h"

// portable structured dot product object
struct dotprod_cccq32_s {
    cq32_t * h;         // coefficients array
    unsigned int n;     // length
};

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
// structured dot product
//

// create structured dot product object
//  _h      :   coefficients array [size: 1 x _n]
//  _n      :   dot product length
dotprod_cccq32 dotprod_cccq32_create(cq32_t * _h,
                                     unsigned int _n)
{
    dotprod_cccq32 q = (dotprod_cccq32) malloc(sizeof(struct dotprod_cccq32_s));
    q->n = _n;

    // allocate memory for coefficients (complex)
    q->h = (cq32_t*) malloc((q->n)*sizeof(cq32_t));

    // move coefficients
    memmove(q->h, _h, (q->n)*sizeof(cq32_t));

    // return object
    return q;
}

// re-create dot product object
//  _q      :   old dot dot product object
//  _h      :   new coefficients [size: 1 x _n]
//  _n      :   new dot product size
dotprod_cccq32 dotprod_cccq32_recreate(dotprod_cccq32 _q,
                                       cq32_t *       _h,
                                       unsigned int   _n)
{
    // check to see if length has changed
    if (_q->n != _n) {
        // set new length
        _q->n = _n;

        // re-allocate memory
        _q->h = (cq32_t*) realloc(_q->h, (_q->n)*sizeof(cq32_t));
    }

    // move new coefficients
    memmove(_q->h, _h, (_q->n)*sizeof(cq32_t));

    // return re-structured object
    return _q;
}

// destroy dot product object
void dotprod_cccq32_destroy(dotprod_cccq32 _q)
{
    free(_q->h);    // free coefficients memory
    free(_q);       // free main object memory
}

// print dot product object
void dotprod_cccq32_print(dotprod_cccq32 _q)
{
    printf("dotprod [%u elements]:\n", _q->n);
    unsigned int i;
    for (i=0; i<_q->n; i++) {
        printf("  %4u: %12.8f + j%12.8f\n", i, q32_fixed_to_float(_q->h[i].real),
                                               q32_fixed_to_float(_q->h[i].imag));
    }
}

// execute structured dot product
//  _q      :   dot product object
//  _x      :   input array [size: 1 x _n]
//  _y      :   output dot product
void dotprod_cccq32_execute(dotprod_cccq32 _q,
                            cq32_t *       _x,
                            cq32_t *       _y)
{
    // run basic dot product with unrolled loops
    dotprod_cccq32_run4(_q->h, _x, _q->n, _y);
}

