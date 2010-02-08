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

#define LIQUID_DEBUG_CHEBY1_PRINT   1

// compute complex roots of a chebyshev filter
void cheby1_rootsf(unsigned int _n,
                   float _ep,
                   float complex *_r)
{
    unsigned int i;

    float nf = (float) _n;

    float t0 = sqrt(1.0 + 1.0/(_ep*_ep));
    float tp = powf( t0 + 1.0/_ep, 1.0/nf );
    float tm = powf( t0 - 1.0/_ep, 1.0/nf );

    float b = 0.5*(tp + tm);    // ellipse major axis
    float a = 0.5*(tp - tm);    // ellipse minor axis

#if LIQUID_DEBUG_CHEBY1_PRINT
    printf("ep : %12.8f\n", _ep);
    printf("b  : %12.8f\n", b);
    printf("a  : %12.8f\n", a);
#endif

    // compute poles
    float theta;
    for (i=0; i<_n; i++) {
        theta = (float)(2*(i+1) + _n - 1)*M_PI/(float)(2*_n);
        _r[i] = -a*cosf(theta) - _Complex_I*b*sinf(theta);
    }
}


void cheby1f(unsigned int _n,
             float _fc,
             float _ep,
             float * _b,
             float * _a)
{
    unsigned int i;
    // poles
    float complex p[_n];
    cheby1_rootsf(_n,_ep,p);

#if LIQUID_DEBUG_CHEBY1_PRINT
    printf("poles:\n");
    for (i=0; i<_n; i++)
        printf("  p[%3u] = %12.8f + j*%12.8f\n", i+1, crealf(-p[i]), cimagf(-p[i]));
#endif

    // compute gain (should be purely real)
    float complex A=1.0f;
    for (i=0; i<_n; i++)
        A *= p[i];
    // gain compensation for even-order filters
    if ( (_n%2) == 0 )
        A *= 1.0f / sqrtf(1.0f + _ep*_ep);
    printf("A : %12.8f + j*%12.8f\n", crealf(A), cimagf(A));

    // normalized cutoff frequency
    float m = 1.0f / tanf(M_PI * _fc);
#if LIQUID_DEBUG_CHEBY1_PRINT
    printf("m = %12.8f\n", m);
#endif

    float complex b[_n+1];  // output numerator
    float complex a[_n+1];  // output denominator

    // compute bilinear z-transform on continuous time
    // transfer function
    bilinear_zpk(NULL,  0,  // zeros
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

