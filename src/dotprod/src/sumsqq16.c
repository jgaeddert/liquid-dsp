/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
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

