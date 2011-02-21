/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Virginia Polytechnic
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
// imdct_execute.c
//
// IMDCT : Inverse Modified Discrete Cosine Transforms
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"


// regular slow transform
void FFT(_execute_IMDCT)(FFT(plan) _p)
{
    // ugly, slow method

    unsigned int n,k;

    // initialize output to zero
    for (n=0; n<2*_p->n; n++)
        _p->xr[n] = 0.0f;

    float inv_n = 1.0f / (float)_p->n;

    float nn;
    for (n=0; n<2*_p->n; n++) {
        for (k=0; k<_p->n; k++) {
            nn = (float)n + 0.5f + (float)_p->n*0.5f;
            _p->xr[n] += _p->yr[k]*cos(M_PI*inv_n*(nn)*(k+0.5f));
        }
    }

    // multiply by window, normalization factor
    for (k=0; k<2*_p->n; k++)
        _p->xr[k] *= _p->w[k] * 2.0f * inv_n;
}

// use internal DCT-IV
void FFT(_execute_IMDCT_REDFT11)(FFT(plan) _p)
{
    // ugly, slow method

    unsigned int n,k;

    // initialize output to zero
    for (n=0; n<2*_p->n; n++)
        _p->xr[n] = 0.0f;

    float inv_n = 1.0f / (float)_p->n;

    if (_p->n % 2) {
        // TODO : use DCT-III
        fprintf(stderr,"error: mdct(), _p->n must be even\n");
        exit(1);
    }
    // compute regular DCT-IV on input
    // TODO : use internal plan
    float x0[_p->n];
    dct_typeIV(_p->yr,x0,_p->n);

    // fold resulting sequence onto itself: create pointers (length _p->n/2)
    unsigned int M = _p->n/2;
    float * e = &x0[0*M];       // -c[M-i-1] - d
    float * f = &x0[1*M];       //  a        - b[M-i-1]

    unsigned int i;
    for (i=0; i<M; i++) {
        _p->xr[i+0*M] =   f[i];     // a - b[M-i-1]
        _p->xr[i+1*M] =  -f[M-i-1]; // b - a[M-i-1]
        _p->xr[i+2*M] =  -e[M-i-1]; // c + d[M-i-1]
        _p->xr[i+3*M] =  -e[i];     // d + c[M-i-1]
    }

    // multiply by window, normalization factor
    for (k=0; k<2*_p->n; k++)
        _p->xr[k] *= _p->w[k] * 2.0f * inv_n;
}

