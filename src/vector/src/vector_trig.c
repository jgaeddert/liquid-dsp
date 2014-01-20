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
#include <math.h>
#include <complex.h>

// compute complex phase rotation: x[i] = exp{ j theta[i] }
//  _theta  :   input primitive array [size: _n x 1]
//  _n      :   array length
//  _x      :   output array pointer [size: _n x 1]
void VECTOR(_cexpj)(TP *         _theta,
                    unsigned int _n,
                    T *          _x)
{
    // t = 4*(floor(_n/4))
    unsigned int t=(_n>>2)<<2; 

    // compute in groups of 4
    unsigned int i;
    for (i=0; i<t; i+=4) {
#if T_COMPLEX
        _x[i  ] = cexpf(_Complex_I*_theta[i  ]);
        _x[i+1] = cexpf(_Complex_I*_theta[i+1]);
        _x[i+2] = cexpf(_Complex_I*_theta[i+2]);
        _x[i+3] = cexpf(_Complex_I*_theta[i+3]);
#else
        _x[i  ] = _theta[i  ] > 0 ? 1.0 : -1.0;
        _x[i+1] = _theta[i+1] > 0 ? 1.0 : -1.0;
        _x[i+2] = _theta[i+2] > 0 ? 1.0 : -1.0;
        _x[i+3] = _theta[i+3] > 0 ? 1.0 : -1.0;
#endif
    }

    // clean up remaining
    for ( ; i<_n; i++) {
#if T_COMPLEX
        _x[i] = cexpf(_Complex_I*_theta[i]);
#else
        _x[i] = _theta[i] > 0 ? 1.0 : -1.0;
#endif
    }
}

// compute complex phase rotation: x[i] = exp{ j theta[i] }
//  _x      :   input array [size: _n x 1]
//  _n      :   array length
//  _theta  :   output primitive array [size: _n x 1]
void VECTOR(_carg)(T *          _x,
                   unsigned int _n,
                   TP *         _theta)
{
    // t = 4*(floor(_n/4))
    unsigned int t=(_n>>2)<<2; 

    // compute in groups of 4
    unsigned int i;
    for (i=0; i<t; i+=4) {
#if T_COMPLEX
        _theta[i  ] = cargf(_x[i  ]);
        _theta[i+1] = cargf(_x[i+1]);
        _theta[i+2] = cargf(_x[i+2]);
        _theta[i+3] = cargf(_x[i+3]);
#else
        _theta[i  ] = _x[i  ] > 0 ? 0 : M_PI;
        _theta[i+1] = _x[i+1] > 0 ? 0 : M_PI;
        _theta[i+2] = _x[i+2] > 0 ? 0 : M_PI;
        _theta[i+3] = _x[i+3] > 0 ? 0 : M_PI;
#endif
    }

    // clean up remaining
    for ( ; i<_n; i++) {
#if T_COMPLEX
        _theta[i] = cargf(_x[i]);
#else
        _theta[i] = _x[i] > 0 ? 0 : M_PI;
#endif
    }
}

// compute absolute value of each element: y[i] = |x[i]|
//  _x      :   input array [size: _n x 1]
//  _n      :   array length
//  _y      :   output primitive array pointer [size: _n x 1]
void VECTOR(_abs)(T *          _x,
                  unsigned int _n,
                  TP *         _y)
{
    // t = 4*(floor(_n/4))
    unsigned int t=(_n>>2)<<2; 

    // compute in groups of 4
    unsigned int i;
    for (i=0; i<t; i+=4) {
#if T_COMPLEX
        _y[i  ] = cabsf(_x[i  ]);
        _y[i+1] = cabsf(_x[i+1]);
        _y[i+2] = cabsf(_x[i+2]);
        _y[i+3] = cabsf(_x[i+3]);
#else
        _x[i  ] = fabsf(_x[i  ]);
        _x[i+1] = fabsf(_x[i+1]);
        _x[i+2] = fabsf(_x[i+2]);
        _x[i+3] = fabsf(_x[i+3]);
#endif
    }

    // clean up remaining
    for ( ; i<_n; i++) {
#if T_COMPLEX
        _y[i] = cabsf(_x[i]);
#else
        _x[i] = fabsf(_x[i]);
#endif
    }
}

