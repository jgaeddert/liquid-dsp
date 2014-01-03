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
// Butterworth filter design
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "liquid.internal.h"

#define LIQUID_DEBUG_BUTTER_PRINT   0

// Compute analog zeros, poles, gain of low-pass Butterworth
// filter, grouping complex conjugates together. If filter
// order is odd, the single real pole (-1) is at the end of
// the array.  There are no zeros for the analog Butterworth
// filter.  The gain is unity.
//  _n      :   filter order
//  _za     :   output analog zeros [length:  0]
//  _pa     :   output analog poles [length: _n]
//  _ka     :   output analog gain
void butter_azpkf(unsigned int _n,
                  liquid_float_complex * _za,
                  liquid_float_complex * _pa,
                  liquid_float_complex * _ka)
{
    unsigned int r = _n%2;
    unsigned int L = (_n - r)/2;
    
    unsigned int i;
    unsigned int k=0;
    for (i=0; i<L; i++) {
        float theta = (float)(2*(i+1) + _n - 1)*M_PI/(float)(2*_n);
        _pa[k++] = cexpf( _Complex_I*theta);
        _pa[k++] = cexpf(-_Complex_I*theta);
    }

    if (r) _pa[k++] = -1.0f;

    assert(k==_n);

    *_ka = 1.0;
}

