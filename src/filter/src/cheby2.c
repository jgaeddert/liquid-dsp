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
// Chebeshev type-I filter design
//

#include <stdio.h>
#include <complex.h>
#include <math.h>
#include <assert.h>
#include <string.h>

#include "liquid.internal.h"

#define LIQUID_DEBUG_CHEBY2_PRINT   1

void cheby2_azpkf(unsigned int _n,
                  float _fc,
                  float _es,
                  liquid_float_complex * _z,
                  liquid_float_complex * _p,
                  liquid_float_complex * _k)
{
    float nf = (float) _n;

    float t0 = sqrt(1.0 + 1.0/(_es*_es));
    float tp = powf( t0 + 1.0/_es, 1.0/nf );
    float tm = powf( t0 - 1.0/_es, 1.0/nf );

    float b = 0.5*(tp + tm);    // ellipse major axis
    float a = 0.5*(tp - tm);    // ellipse minor axis

#if LIQUID_DEBUG_CHEBY1_PRINT
    printf("ep : %12.8f\n", _es);
    printf("b  : %12.8f\n", b);
    printf("a  : %12.8f\n", a);
#endif

    unsigned int r = _n%2;
    unsigned int L = (_n - r)/2;
    
    // compute poles
    unsigned int i;
    unsigned int k=0;
    for (i=0; i<L; i++) {
        float theta = (float)(2*(i+1) + _n - 1)*M_PI/(float)(2*_n);
        _p[k++] = 1.0f / (a*cosf(theta) - _Complex_I*b*sinf(theta));
        _p[k++] = 1.0f / (a*cosf(theta) + _Complex_I*b*sinf(theta));
    }

    if (r) _p[k++] = -1.0f / a;

    assert(k==_n);

    // compute zeros
    k=0;
    for (i=0; i<L; i++) {
        //float theta = (float)(2*(i+1) + _n - 1)*M_PI/(float)(2*_n);
        float theta = (float)(0.5f*M_PI*(2*(i+1)-1)/(float)(_n));
        _z[k++] = -1.0f / (_Complex_I*cosf(theta));
        _z[k++] =  1.0f / (_Complex_I*cosf(theta));
    }

    assert(k==2*L);

#if 0
    // compute gain
    float complex Az=1.0f;
    float complex Ap=1.0f;
    for (i=0; i<2*L; i++)
        Az *= _z[i];
    for (i=0; i<_n; i++)
        Ap *= _p[i];

    *_k = Ap / Az;
#else
    *_k = 1.0f;
#endif
}

