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
// MDCT : Modified Discrete Cosine Transforms
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

// 
void mdct(float * _x,
          float * _X,
          float * _w,
          unsigned int _N)
{
    // ugly, slow method

    // pre-compute windowed input
    float xw[2*_N];

    unsigned int n,k;
    for (n=0; n<2*_N; n++)
        xw[n] = _x[n] * _w[n];

    // initialize output to zero
    for (k=0; k<_N; k++)
        _X[k] = 0.0f;

#if 1
    float nn;
    float inv_N = 1.0f / (float)_N;
    for (k=0; k<_N; k++) {
        for (n=0; n<2*_N; n++) {
            nn = (float)n + 0.5f + (float)_N*0.5f;
            _X[k] += xw[n]*cosf(M_PI*inv_N*(nn)*(0.5f+k));
        }
    }
#else
    if (_N % 2) {
        // TODO : use DCT-II
        fprintf(stderr,"error: mdct(), _N must be even\n");
        exit(1);
    }
    // fold input sequence onto itself: create pointers (length _N/2)
    unsigned int M = _N/2;
    float * a = &xw[0*M];
    float * b = &xw[1*M];
    float * c = &xw[2*M];
    float * d = &xw[3*M];

    float x0[_N];
    unsigned int i;
    for (i=0; i<M; i++) {
        x0[i]   = -c[M-i-1] - d[i];
        x0[i+M] =  a[i]     - b[M-i-1];
    }

    // compute regular DCT-IV on result
    dct_typeIV(x0,_X,_N);
#endif
}

// 
void imdct(float * _X,
           float * _x,
           float * _w,
           unsigned int _N)
{
    // ugly, slow method

    unsigned int n,k;

    // initialize output to zero
    for (n=0; n<2*_N; n++)
        _x[n] = 0.0f;

    float inv_N = 1.0f / (float)_N;
#if 0
    float nn;
    for (n=0; n<2*_N; n++) {
        for (k=0; k<_N; k++) {
            nn = (float)n + 0.5f + (float)_N*0.5f;
            _x[n] += _X[k]*cos(M_PI*inv_N*(nn)*(k+0.5f));
        }
    }
#else
    if (_N % 2) {
        // TODO : use DCT-III
        fprintf(stderr,"error: mdct(), _N must be even\n");
        exit(1);
    }
    // compute regular DCT-IV on input
    float x0[_N];
    dct_typeIV(_X,x0,_N);

    // fold resulting sequence onto itself: create pointers (length _N/2)
    unsigned int M = _N/2;
    float * e = &x0[0*M];       // -c[M-i-1] - d
    float * f = &x0[1*M];       //  a        - b[M-i-1]

    unsigned int i;
    for (i=0; i<M; i++) {
        _x[i+0*M] =   f[i];     // a - b[M-i-1]
        _x[i+1*M] =  -f[M-i-1]; // b - a[M-i-1]
        _x[i+2*M] =  -e[M-i-1]; // c + d[M-i-1]
        _x[i+3*M] =  -e[i];     // d + c[M-i-1]
    }
#endif

    // multiply by window, normalization factor
    for (k=0; k<2*_N; k++)
        _x[k] *= _w[k] * 2.0f * inv_N;
}

