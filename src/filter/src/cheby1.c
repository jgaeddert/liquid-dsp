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
// Chebeshev type-I filter design
//

#include <stdio.h>
#include <complex.h>
#include <math.h>
#include <assert.h>
#include <string.h>

#include "liquid.internal.h"

#define LIQUID_DEBUG_CHEBY1_PRINT   0

// Compute analog zeros, poles, gain of low-pass Chebyshev
// Type I filter, grouping complex conjugates together. If
// the filter order is odd, the single real pole is at the
// end of the array.  There are no zeros for the analog
// Chebyshev Type I filter.
//  _n      :   filter order
//  _ep     :   epsilon, related to pass-band ripple
//  _za     :   output analog zeros [length:  0]
//  _pa     :   output analog poles [length: _n]
//  _ka     :   output analog gain
void cheby1_azpkf(unsigned int _n,
                  float _ep,
                  liquid_float_complex * _za,
                  liquid_float_complex * _pa,
                  liquid_float_complex * _ka)
{
    // temporary values
    float t0 = sqrt(1.0 + 1.0/(_ep*_ep));
    float tp = powf( t0 + 1.0/_ep, 1.0/(float)(_n) );
    float tm = powf( t0 - 1.0/_ep, 1.0/(float)(_n) );

    float b = 0.5*(tp + tm);    // ellipse major axis
    float a = 0.5*(tp - tm);    // ellipse minor axis

#if LIQUID_DEBUG_CHEBY1_PRINT
    printf("  ep : %12.8f\n", _ep);
    printf("  b  : %12.8f\n", b);
    printf("  a  : %12.8f\n", a);
#endif

    // filter order variables
    unsigned int r = _n%2;          // odd order?
    unsigned int L = (_n - r)/2;    // half order
    
    // compute poles
    unsigned int i;
    unsigned int k=0;
    for (i=0; i<L; i++) {
        float theta = (float)(2*(i+1) + _n - 1)*M_PI/(float)(2*_n);
        _pa[k++] = a*cosf(theta) - _Complex_I*b*sinf(theta);
        _pa[k++] = a*cosf(theta) + _Complex_I*b*sinf(theta);
    }

    // if filter order is odd, there is an additional pole on the
    // real axis
    if (r) _pa[k++] = -a;

    // ensure we have written exactly _n poles
    assert(k==_n);

    // compute analog gain (ignored in digital conversion)
    *_ka = r ? 1.0f : 1.0f / sqrtf(1.0f + _ep*_ep);
    for (i=0; i<_n; i++)
        *_ka *= _pa[i];
}

