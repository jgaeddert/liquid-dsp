/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
 *
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
// Finite impulse response filter design
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

void scramble_data(unsigned char * _x,
                   unsigned int _n)
{
    // t = 4*(floor(_n/4))
    unsigned int t = (_n>>2)<<2;

    // apply static masks
    unsigned int i;
    for (i=0; i<t; i+=4) {
        _x[i  ] ^= LIQUID_SCRAMBLE_MASK0;
        _x[i+1] ^= LIQUID_SCRAMBLE_MASK1;
        _x[i+2] ^= LIQUID_SCRAMBLE_MASK2;
        _x[i+3] ^= LIQUID_SCRAMBLE_MASK3;
    }

    // clean up remainder of elements
    for ( ; i<_n; i++)
        _x[i] ^= LIQUID_SCRAMBLE_MASK0;
}

void unscramble_data(unsigned char * _x,
                     unsigned int _n)
{
    // for now apply simple static mask (re-run scramble)
    scramble_data(_x,_n);
}

