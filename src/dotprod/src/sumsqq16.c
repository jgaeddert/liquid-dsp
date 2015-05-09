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
// sumsq.c : sum of squares
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "liquid.internal.h"

// sum squares, basic loop
//  _v      :   input array [size: 1 x _n]
//  _n      :   input length
q16_t liquid_sumsqq16(q16_t *      _v,
                      unsigned int _n)
{
    // initialize accumulator
    q16_at r=0;

    // t = 4*(floor(_n/4))
    unsigned int t=(_n>>2)<<2; 

    // run computation in groups of 4
    unsigned int i;
    for (i=0; i<t; i+=4) {
        r += (q16_at)_v[i  ] * (q16_at)_v[i  ];
        r += (q16_at)_v[i+1] * (q16_at)_v[i+1];
        r += (q16_at)_v[i+2] * (q16_at)_v[i+2];
        r += (q16_at)_v[i+3] * (q16_at)_v[i+3];
    }

    // clean up remaining
    for ( ; i<_n; i++)
        r += (q16_at)_v[i] * (q16_at)_v[i];

    // return result
    return (r >> q16_fracbits);
}

// sum squares, basic loop
//  _v      :   input array [size: 1 x _n]
//  _n      :   input length
q16_t liquid_sumsqcq16(cq16_t *     _v,
                       unsigned int _n)
{
    // simple method: type cast input as real pointer, run double
    // length sumsqf method
    q16_t * v = (q16_t*) _v;
    return liquid_sumsqq16(v, 2*_n);
}

