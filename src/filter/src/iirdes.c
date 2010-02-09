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
// iir (infinite impulse response) filter design
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "liquid.internal.h"

// converts discrete-time zero/pole/gain (zpk) recursive (iir)
// filter representation to second-order sections (sos) form
//
//  _z      :   zeros array (size _nz)
//  _nz     :   number of zeros
//  _p      :   poles array (size _np)
//  _np     :   number of poles
//  _k      :   gain
//
//  _B      :   output numerator matrix (size L x 3)
//  _A      :   output denominator matrix (size L x 3)
//
//  L is the number of sections in the cascade:
//      L = _np % 2 ? (_np + 1)/2 : _np/2;
void iirdes_zpk2sos(float complex * _z,
                    unsigned int _nz,
                    float complex * _p,
                    unsigned int _np,
                    float _k,
                    float * _B,
                    float * _A)
{
    // find/group complex conjugate pairs (poles)

    // find/group complex conjugate pairs (zeros)

    // group poles pairs with zeros pairs
}

