/*
 * Copyright (c) 2011 Joseph Gaeddert
 * Copyright (c) 2011 Virginia Polytechnic Institute & State University
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

#include "liquid.internal.h"

// compute the linear prediction coefficients for an input signal _x
//  _x      :   input signal [size: _n x 1]
//  _p      :   prediction filter order
//  _a      :   prediction filter [size: _p+1 x 1]
//  _g      :   .
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

#if 0
    // TODO : solve the Toeplitz inversion using Levinson-Durbin recursion
    for (i=0; i<_p+1; i++) {
        
    }
#else
    // use low inversion method
    float R[_p*_p];
    for (i=0; i<_p; i++) {
        unsigned int j;
        for (j=0; j<_p; j++)
            matrix_access(R,_p,_p,i,j) = r[abs(i-j)];
    }

    // invert matrix (using Gauss-Jordan elimination)
    matrixf_inv(R,_p,_p);

    float rt[_p];
    float a_hat[_p];

    for (i=0; i<_p; i++)
        rt[i] = -r[i+1];

    // multiply R_inv with r to get _a vector
    matrixf_mul(R,     _p, _p,
                rt,    _p, 1,
                a_hat, _p, 1);

    // copy coefficients
    _a[0] = 1.0f;
    for (i=0; i<_p; i++)
        _a[i+1] = a_hat[i];

    for (i=0; i<_p+1; i++)
        _g[i] = 0.0f;
#endif
}


