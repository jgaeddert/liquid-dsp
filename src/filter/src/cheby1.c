/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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

void cheby1f(unsigned int _n, float _ep, float * _b, float * _a)
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
    float complex s[_n];
    float theta;
    for (i=0; i<_n; i++) {
        theta = (float)(2*(i+1) + _n - 1)*M_PI/(float)(2*_n);

        //_r[i] = -cexpf(_Complex_I*theta); // butterworth
        s[i] = -a*cosf(theta) - _Complex_I*b*sinf(theta);
    }

#if LIQUID_DEBUG_CHEBY1_PRINT
    printf("poles:\n");
    for (i=0; i<_n; i++)
        printf("  s[%3u] = %12.8f + j*%12.8f\n", i+1, crealf(-s[i]), cimagf(-s[i]));
#endif

    // compute gain (should be purely real)
    float complex A=1.0f;
    for (i=0; i<_n; i++)
        A *= s[i];
    A *= 1.0f / sqrtf(1.0f + _ep*_ep);
    printf("A : %12.8f + j*%12.8f\n", crealf(A), cimagf(A));

    // expand roots
    float complex pcf[_n+1];
    cfpoly_expandroots(s,_n,pcf);

    float p[_n+1];
    for (i=0; i<=_n; i++)
        p[i] = crealf(pcf[i]);

#if LIQUID_DEBUG_CHEBY1_PRINT
    printf("expanded polynomial:\n");
    for (i=0; i<=_n; i++)
        printf("  p[%3u] = %12.8f + j*%12.8f\n", i+1, crealf(pcf[i]), cimagf(pcf[i]));
        //printf("  p[%3u] = %12.8f\n", i+1, p[i]);
#endif
    // normalized cutoff frequency
    float _fc = 0.25f;
    float m = 1.0f / tanf(M_PI * _fc);
    float mk = 1.0f;    // placeholder for m^k
#if LIQUID_DEBUG_CHEBY1_PRINT
    printf("m = %12.8f\n", m);
#endif

    // clear output 
    memset(_b, 0, (_n+1)*sizeof(float));
    memset(_a, 0, (_n+1)*sizeof(float));

    // temporary polynomial: (1 + 1/z)^(k) * (1 - 1/z)^(n-k)
    int poly_1pz[_n+1];

    // compute denominator
    unsigned int j;
    for (i=0; i<=_n; i++) {
        // expand the polynomial (1 + x)^i * (1 - x)^(_n-i)
        poly_binomial_expand_pm(_n,_n-i,poly_1pz);

#if LIQUID_DEBUG_CHEBY1_PRINT
        printf("  poly[n=%-3u k=%-3u] : ", _n, i);
        for (j=0; j<=_n; j++)
            printf("%6d", poly_1pz[j]);
        printf("\n");
#endif

        // accumulate polynomial coefficients
        for (j=0; j<=_n; j++)
            _a[j] += p[i]*mk*poly_1pz[j];

        // update frequency-scaling multiplier
        mk *= m;
    }

    // compute numerator (simple binomial expansion)
    poly_binomial_expand(_n,poly_1pz);
    for (i=0; i<=_n; i++)
        _b[i] = poly_1pz[i];

    // normalize by a[0]
    float a0_inv = 1.0f / _a[0];
    for (i=0; i<=_n; i++) {
        _b[i] *= a0_inv * crealf(A);
        _a[i] *= a0_inv;
    }

}

