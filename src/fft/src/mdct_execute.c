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
// mdct_execute.c
//
// MDCT : Modified Discrete Cosine Transforms
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

// regular slow transform
void FFT(_execute_MDCT)(FFT(plan) _p)
{
    // ugly, slow method

    // pre-compute windowed input
    float xw[2*_p->n];

    unsigned int n,k;
    for (n=0; n<2*_p->n; n++)
        xw[n] = _p->xr[n] * _p->w[n];

    // initialize output to zero
    for (k=0; k<_p->n; k++)
        _p->yr[k] = 0.0f;

    float nn;
    float inv_n = 1.0f / (float)_p->n;
    for (k=0; k<_p->n; k++) {
        for (n=0; n<2*_p->n; n++) {
            nn = (float)n + 0.5f + (float)_p->n*0.5f;
            _p->yr[k] += xw[n]*cosf(M_PI*inv_n*(nn)*(0.5f+k));
        }
    }
}


// use internal DCT-IV
void FFT(_execute_MDCT_REDFT11)(FFT(plan) _p)
{
    // ugly, slow method

    // pre-compute windowed input
    float xw[2*_p->n];

    unsigned int n,k;
    for (n=0; n<2*_p->n; n++)
        xw[n] = _p->xr[n] * _p->w[n];

    // initialize output to zero
    for (k=0; k<_p->n; k++)
        _p->yr[k] = 0.0f;

    if (_p->n % 2) {
        // TODO : use DCT-II
        fprintf(stderr,"error: mdct(), _p->n must be even\n");
        exit(1);
    }
    // fold input sequence onto itself: create pointers (length _p->n/2)
    unsigned int M = _p->n/2;
    float * a = &xw[0*M];
    float * b = &xw[1*M];
    float * c = &xw[2*M];
    float * d = &xw[3*M];

    float x0[_p->n];
    unsigned int i;
    for (i=0; i<M; i++) {
        x0[i]   = -c[M-i-1] - d[i];
        x0[i+M] =  a[i]     - b[M-i-1];
    }

    // compute regular DCT-IV on result
    // TODO : use internal plan
    dct_typeIV(x0,_p->yr,_p->n);
}


