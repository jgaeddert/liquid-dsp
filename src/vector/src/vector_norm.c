/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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
#include <math.h>

// compute l2-norm on vector
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

// scale vector to its l2-norm
//  _x      :   input array [size: _n x 1]
//  _n      :   array length
//  _y      :   output array [size: _n x 1]
void VECTOR(_normalize)(T *          _x,
                        unsigned int _n,
                        T *          _y)
{
    // compute l2-norm on vector
    TP norm = VECTOR(_norm)(_x, _n);

    // compute inverse
    TP norm_inv = 1.0 / norm;

    // t = 4*(floor(_n/4))
    unsigned int t=(_n>>2)<<2; 

    // scale by inverse; compute in groups of 4
    unsigned int i;
    for (i=0; i<t; i+=4) {
        _y[i  ] = _x[i  ] * norm_inv;
        _y[i+1] = _x[i+1] * norm_inv;
        _y[i+2] = _x[i+2] * norm_inv;
        _y[i+3] = _x[i+3] * norm_inv;
    }

    // clean up remaining
    for ( ; i<_n; i++)
        _y[i] = _x[i] * norm_inv;
}

