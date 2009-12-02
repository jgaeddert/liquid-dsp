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
// MDCT : Modified Discrete Cosine Transforms
//

#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

// 
void mdct(float * _x, float * _X, float * _w, unsigned int _N)
{
    // ugly, slow method

    // pre-compute windowed input
    float xw[2*_N];

    float inv_N = 1.0f / (float)_N;
    unsigned int n,k;
    for (n=0; n<2*_N; n++)
        xw[n] = _x[n] * _w[n];

    // initialize output to zero
    for (k=0; k<_N; k++)
        _X[k] = 0.0f;

    float nn;
    for (k=0; k<_N; k++) {
        for (n=0; n<2*_N; n++) {
            nn = (float)n + 0.5f + (float)_N*0.5f;
            _X[k] += xw[n]*cosf(M_PI*inv_N*(nn)*(0.5f+k));
        }
    }
}

// 
void imdct(float * _X, float * _x, float * _w, unsigned int _N)
{
    // ugly, slow method

    float inv_N = 1.0f / (float)_N;

    unsigned int n,k;

    // initialize output to zero
    for (n=0; n<2*_N; n++)
        _x[n] = 0.0f;

    float nn;
    for (k=0; k<_N; k++) {
        for (n=0; n<2*_N; n++) {
            nn = (float)n + 0.5f + (float)_N*0.5f;
            _x[k] += _X[k]*cos(M_PI*inv_N*(nn)*(k+0.5f));
        }
    }

    // multiply by window
    for (k=0; k<2*_N; k++)
        _x[k] *= _w[k];
}

