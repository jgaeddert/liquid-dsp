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
// Butterworth filter design
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "liquid.internal.h"

#define LIQUID_DEBUG_BUTTER_PRINT   0

// compute complex roots of the butterworth polynomial (poles of the
// filter); length of _r is _n+1
void butter_rootsf(unsigned int _n,
                   float complex *_r)
{
    unsigned int i;
    float theta;
    for (i=0; i<_n; i++) {
        theta = (float)(2*(i+1) + _n - 1)*M_PI/(float)(2*_n);
        _r[i] = -cexpf(_Complex_I*theta);
    }

#if LIQUID_DEBUG_BUTTER_PRINT
    printf("poles:\n");
    for (i=0; i<_n; i++)
        printf("  r[%3u] = %12.8f + j*%12.8f\n", i+1, crealf(-_r[i]), cimagf(-_r[i]));
#endif
}

// butterworth polynomial of order _n (length of _p is _n+1)
void butter_polyf(unsigned int _n, float *_p)
{
    // compute poles
    float complex s[_n];
    butter_rootsf(_n,s);
    
    // expand roots
    float complex pcf[_n+1];
    cfpoly_expandroots(s,_n,pcf);

    // return only real components
    unsigned int i;
    for (i=0; i<=_n; i++)
        _p[i] = crealf(pcf[i]);
}

void butterf(unsigned int _n,
             float _fc,
             float * _b,
             float * _a)
{
    // validate input
    if (_fc <= 0.0f || _fc >= 0.5f) {
        fprintf(stderr,"error: butterf(), cutoff frequency out of range\n");
        exit(1);
    } else if (_n == 0) {
        fprintf(stderr,"error: butterf(), filter order must be greater than zero\n");
        exit(1);
    }

    unsigned int i, j;
    // butterworth polynomial
    float p[_n+1];
    butter_polyf(_n,p);

#if LIQUID_DEBUG_BUTTER_PRINT
    printf("expanded polynomial:\n");
    for (i=0; i<=_n; i++)
        printf("  p[%3u] = %12.8f\n", i+1, p[i]);
#endif

    // ...
    unsigned int nb = _n+1;
    unsigned int na = _n+1;

    // normalized cutoff frequency
    float m = 1.0f / tanf(M_PI * _fc);
    float mk = 1.0f;    // placeholder for m^k
#if LIQUID_DEBUG_BUTTER_PRINT
    printf("m = %12.8f\n", m);
#endif

    for (i=0; i<nb; i++) _b[i] = 0;
    for (i=0; i<na; i++) _a[i] = 0;

    // temporary polynomial: (1 + 1/z)^(k) * (1 - 1/z)^(n-k)
    int poly_1pz[na];

    // compute denominator
    for (i=0; i<na; i++) {
        // expand the polynomial (1 + x)^i * (1 - x)^(_n-i)
        poly_binomial_expand_pm(_n,_n-i,poly_1pz);

#if LIQUID_DEBUG_BUTTER_PRINT
        printf("  poly[n=%-3u k=%-3u] : ", _n, i);
        for (j=0; j<na; j++)
            printf("%6d", poly_1pz[j]);
        printf("\n");
#endif

        // accumulate polynomial coefficients
        for (j=0; j<na; j++)
            _a[j] += p[i]*mk*poly_1pz[j];

        // update frequency-scaling multiplier
        mk *= m;
    }

    // compute numerator (simple binomial expansion)
    poly_binomial_expand(_n,poly_1pz);
    for (i=0; i<na; i++)
        _b[i] = poly_1pz[i];

    // normalize by a[0]
    float a0_inv = 1.0f / _a[0];
    for (i=0; i<=_n; i++) {
        _b[i] *= a0_inv;
        _a[i] *= a0_inv;
    }
     
#if LIQUID_DEBUG_BUTTER_PRINT
    printf("numerator:\n");
    for (i=0; i<nb; i++)
        printf("  b[%3u] = %12.8f\n", i, _b[i]);
    printf("denominator:\n");
    for (i=0; i<na; i++)
        printf("  a[%3u] = %12.8f\n", i, _a[i]);
#endif
}

