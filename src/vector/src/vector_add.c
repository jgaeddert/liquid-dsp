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
// Generic vector addition
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// basic vector addition, unrolling loop
//  _x      :   first array  [size: _n x 1]
//  _y      :   second array [size: _n x 1]
//  _n      :   array lengths
//  _z      :   output array pointer [size: _n x 1]
void VECTOR(_add)(T *          _x,
                  T *          _y,
                  unsigned int _n,
                  T *          _z)
{
    // t = 4*(floor(_n/4))
    unsigned int t=(_n>>2)<<2; 

    // compute in groups of 4
    unsigned int i;
    for (i=0; i<t; i+=4) {
        _z[i  ] = _x[i  ] + _y[i  ];
        _z[i+1] = _x[i+1] + _y[i+1];
        _z[i+2] = _x[i+2] + _y[i+2];
        _z[i+3] = _x[i+3] + _y[i+3];
    }

    // clean up remaining
    for ( ; i<_n; i++)
        _z[i] = _x[i] + _y[i];
}

// basic vector scalar addition, unrolling loop
//  _x      :   input array  [size: _n x 1]
//  _n      :   array length
//  _v      :   scalar
//  _y      :   output array pointer [size: _n x 1]
void VECTOR(_addscalar)(T *          _x,
                        unsigned int _n,
                        T            _v,
                        T *          _y)
{
    // t = 4*(floor(_n/4))
    unsigned int t=(_n>>2)<<2; 

    // compute in groups of 4
    unsigned int i;
    for (i=0; i<t; i+=4) {
        _y[i  ] = _x[i  ] + _v;
        _y[i+1] = _x[i+1] + _v;
        _y[i+2] = _x[i+2] + _v;
        _y[i+3] = _x[i+3] + _v;
    }

    // clean up remaining
    for ( ; i<_n; i++)
        _y[i] = _x[i] + _v;
}

