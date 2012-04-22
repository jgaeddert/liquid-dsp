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
// DCT : Discrete Cosine Transforms
//

#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

// real, even DFT: DCT-II
void dct(float * _x,
         float * _y,
         unsigned int _n)
{
    // ugly, slow method
    unsigned int i,k;
    float n_inv = 1.0f / (float)_n;
    float phi;
    for (i=0; i<_n; i++) {
        _y[i] = 0.0f;
        for (k=0; k<_n; k++) {
            phi = M_PI*n_inv*((float)k + 0.5f)*i;
            _y[i] += _x[k]*cosf(phi);
        }
    }
}

// real, even iDFT: DCT-III
void idct(float * _x,
          float * _y,
          unsigned int _n)
{
    // ugly, slow method
    unsigned int i,k;
    float n_inv = 1.0f / (float)_n;
    float phi;
    for (i=0; i<_n; i++) {
        _y[i] = _x[0]*0.5f;
        for (k=1; k<_n; k++) {
            phi = M_PI*n_inv*((float)i + 0.5f)*k;
            _y[i] += _x[k]*cosf(phi);
        }
    }
}

// DCT-IV
void dct_typeIV(float * _x,
                float * _y,
                unsigned int _n)
{
    // ugly, slow method
    unsigned int i,k;

    // compute 1/n
    float n_inv = 1.0f / (float)_n;

    float phi;
    for (i=0; i<_n; i++) {
        _y[i] = 0.0f;
        for (k=0; k<_n; k++) {
            phi = M_PI*n_inv*((float)i + 0.5f)*((float)k + 0.5f);
            _y[i] += _x[k]*cosf(phi);
        }
    }
}

