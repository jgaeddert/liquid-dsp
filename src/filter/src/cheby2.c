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
#include <string.h>

#include "liquid.internal.h"

#define LIQUID_DEBUG_CHEBY2_PRINT   1

// compute complex poles of a Chebyshev type II filter
void cheby2_polesf(unsigned int _n,
                   float _ep,
                   float complex *_p)
{
    unsigned int i;

    float nf = (float) _n;

    float t0 = sqrt(1.0 + 1.0/(_ep*_ep));
    float tp = powf( t0 + 1.0/_ep, 1.0/nf );
    float tm = powf( t0 - 1.0/_ep, 1.0/nf );

    float b = 0.5*(tp + tm);    // ellipse major axis
    float a = 0.5*(tp - tm);    // ellipse minor axis

#if LIQUID_DEBUG_CHEBY2_PRINT
    printf("ep : %12.8f\n", _ep);
    printf("b  : %12.8f\n", b);
    printf("a  : %12.8f\n", a);
#endif

    // compute poles
    float theta;
    for (i=0; i<_n; i++) {
        theta = (float)(2*(i+1) + _n - 1)*M_PI/(float)(2*_n);
        _p[i] = 1.0f / (-a*cosf(theta) - _Complex_I*b*sinf(theta));
    }
}

// compute complex zeros of a Chebyshev type II filter
void cheby2_zerosf(unsigned int _n,
                   float _ep,
                   float complex *_z)
{
    unsigned int i;

    // compute zeros
    float theta;
    for (i=0; i<_n; i++) {
        theta = (float)(0.5f*M_PI*(2*(i+1)-1)/(float)(_n));
        //printf("theta : %12.8f\n", theta);
        _z[i] = -1.0f / (_Complex_I*cosf(theta));
    }
}


void cheby2f(unsigned int _n,
             float _fc,
             float _ep,
             float * _b,
             float * _a)
{
    unsigned int i;
    // poles
    float complex p[_n];
    cheby2_polesf(_n,_ep,p);

    // zeros
    float complex z[_n];
    cheby2_zerosf(_n,_ep,z);

#if LIQUID_DEBUG_CHEBY2_PRINT
    printf("poles:\n");
    for (i=0; i<_n; i++)
        printf("  p[%3u] = %12.8f + j*%12.8f\n", i+1, crealf(-p[i]), cimagf(-p[i]));
    printf("zeros:\n");
    for (i=0; i<_n; i++)
        printf("  z[%3u] = %12.8f + j*%12.8f\n", i+1, crealf(z[i]), cimagf(z[i]));
#endif

    // compute gain
    float complex A=_ep;

    // normalized cutoff frequency
    float m = 1.0f / tanf(M_PI * _fc);

#if LIQUID_DEBUG_CHEBY2_PRINT
    printf("A : %12.8f + j*%12.8f\n", crealf(A), cimagf(A));
    printf("m = %12.8f\n", m);
#endif

    float complex b[_n+1];  // output numerator
    float complex a[_n+1];  // output denominator

    // compute bilinear z-transform on continuous time
    // transfer function
    bilinear_zpk(z,     _n, // zeros
                 p,     _n, // poles
                 A,     m,  // scaling/warping factors
                 b,     a); // output

    // retain only real component (imaginary should
    // be zero since poles are all complementary
    // complex pairs)
    for (i=0; i<=_n; i++) {
        _b[i] = crealf(b[i]);
        _a[i] = crealf(a[i]);
    }

}

