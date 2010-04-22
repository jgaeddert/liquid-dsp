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
// DCT : Discrete Cosine Transforms
//

#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

// real, even DFT: DCT-II
void fft_execute_REDCT01_dct(fftplan _p)
{
    // ugly, slow method
    unsigned int i,k;
    float n_inv = 1.0f / (float)_p->n;
    float phi;
    for (i=0; i<_p->n; i++) {
        _p->yr[i] = 0.0f;
        for (k=0; k<_p->n; k++) {
            phi = M_PI*n_inv*((float)k + 0.5f)*i;
            _p->yr[i] += _p->xr[k]*cosf(phi);
        }
    }
}

// real, even iDFT: DCT-III
void fft_execute_REDCT10_dct(fftplan _p)
{
    // ugly, slow method
    unsigned int i,k;
    float n_inv = 1.0f / (float)_p->n;
    float phi;
    for (i=0; i<_p->n; i++) {
        _p->yr[i] = _p->xr[0]*0.5f;
        for (k=1; k<_p->n; k++) {
            phi = M_PI*n_inv*((float)i + 0.5f)*k;
            _p->yr[i] += _p->xr[k]*cosf(phi);
        }
    }
}
