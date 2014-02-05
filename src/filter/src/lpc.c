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
// lpc.c
//
// linear prediction coefficients
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.internal.h"

// compute the linear prediction coefficients for an input signal _x
//  _x      :   input signal [size: _n x 1]
//  _p      :   prediction filter order
//  _a      :   prediction filter [size: _p+1 x 1]
//  _e      :   prediction error variance [size: _p+1 x 1]
void liquid_lpc(float * _x,
                unsigned int _n,
                unsigned int _p,
                float * _a,
                float * _g)
{
    // validate input
    if (_p > _n) {
        fprintf(stderr,"error: liquid_lpc(), prediction filter length cannot exceed input signal length\n");
        exit(1);
    }

    // compute auto-correlation with lags
    float r[_p+1];    // auto-correlation array

    unsigned int i;
    for (i=0; i<_p+1; i++) {
        unsigned int lag = i;
        unsigned int j;
        r[i] = 0.0f;
        for (j=lag; j<_n; j++)
            r[i] += _x[j] * _x[j-lag];
        printf("r[%3u] = %12.8f\n", i, r[i]);
    }

    // solve the Toeplitz inversion using Levinson-Durbin recursion
    liquid_levinson(r,_p,_a,_g);
}


// solve the Yule-Walker equations using Levinson-Durbin recursion
// for _symmetric_ autocorrelation
//  _r      :   autocorrelation array [size: _p+1 x 1]
//  _p      :   filter order
//  _a      :   output coefficients [size: _p+1 x 1]
//  _e      :   error variance [size: _p+1 x 1]
//
// NOTES:
//  By definition _a[0] = 1.0
void liquid_levinson(float * _r,
                     unsigned int _p,
                     float * _a,
                     float * _e)
{
    float a0[_p+1]; // temporary coefficients array, index [n]
    float a1[_p+1]; // temporary coefficients array, index [n-1]
    float e[_p+1];  // prediction error
    float k[_p+1];  // reflection coefficients

    // initialize
    k[0] = 1.0f;
    e[0] = _r[0];

    unsigned int i;
    for (i=0; i<_p+1; i++) {
        a0[i] = (i==0) ? 1.0f : 0.0f;
        a1[i] = (i==0) ? 1.0f : 0.0f;
    }

    unsigned int n;
    for (n=1; n<_p+1; n++) {

        float q = 0.0f;
        for (i=0; i<n; i++)
            q += a0[i]*_r[n-i];

        k[n] = -q/e[n-1];
        e[n] = e[n-1]*(1.0f - k[n]*k[n]);

        // compute new coefficients
        for (i=0; i<n; i++)
            a1[i] = a0[i] + k[n]*a0[n-i];

        a1[n] = k[n];
#if 0
        printf("iteration [n=%u]\n", n);
        for (i=0; i<n; i++)
            printf("  ** i = %3u, n-i = %3u\n", i, n-i);

        // print results
        printf("  k   = %12.8f\n", k[n]);
        printf("  q   = %12.8f\n", q);
        printf("  e   = %12.8f\n", e[n]);

        printf("  a_%u = {", n-1);
        for (i=0; i<n; i++)
            printf("%6.3f, ", a0[i]);
        printf("}\n");

        printf("  a_%u = {", n);
        for (i=0; i<n+1; i++)
            printf("%6.3f, ", a1[i]);
        printf("}\n");
#endif

        // copy temporary vector a1 to a0
        memmove(a0, a1, (_p+1)*sizeof(float));

    }

    // copy results to output
    memmove(_a, a1, (_p+1)*sizeof(float));
    memmove(_e,  e, (_p+1)*sizeof(float));
}

