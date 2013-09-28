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
// Generic vector norm computation
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// basic vector multiplication, unrolling loop
//  _x      :   input array [size: _n x 1]
//  _n      :   array length
TP VECTOR(_norm)(T *          _x,
                 unsigned int _n)
{
    // t = 4*(floor(_n/4))
    unsigned int t=(_n>>2)<<2; 

    // initialize accumulator
    TP norm = 0;

    // compute in groups of 4
    // TODO: use generic 'real' and 'conj' functions
    unsigned int i;
    for (i=0; i<t; i+=4) {
        norm += crealf( _x[i  ]*conjf(_x[i  ]) );
        norm += crealf( _x[i+1]*conjf(_x[i+1]) );
        norm += crealf( _x[i+2]*conjf(_x[i+2]) );
        norm += crealf( _x[i+3]*conjf(_x[i+3]) );
    }

    // clean up remaining
    // TODO: use generic 'real' and 'conj' functions
    for ( ; i<_n; i++)
        norm += crealf( _x[i]*conjf(_x[i]) );

    // return square root of accumulation
    // TODO: use generic 'sqrt' function
    return sqrtf(norm);
}

