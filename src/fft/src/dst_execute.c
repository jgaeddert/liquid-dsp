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
// DST : Discrete Sine Transforms
//

#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

// DST-I
void FFT(_execute_RODFT00)(FFT(plan) _p)
{
    // ugly, slow method
    unsigned int i,k;
    float n_inv = 1.0f / (float)(_p->n+1);
    float phi;
    for (i=0; i<_p->n; i++) {
        _p->yr[i] = 0.0f;
        for (k=0; k<_p->n; k++) {
            phi = M_PI*n_inv*(float)((k+1)*(i+1));
            _p->yr[i] += _p->xr[k]*sinf(phi);
        }

        // compensate for discrepancy
        _p->yr[i] *= 2.0f;
    }
}

// DST-II
void FFT(_execute_RODFT10)(FFT(plan) _p)
{
    // ugly, slow method
    unsigned int i,k;
    float n_inv = 1.0f / (float)(_p->n);
    float phi;
    for (i=0; i<_p->n; i++) {
        _p->yr[i] = 0.0f;
        for (k=0; k<_p->n; k++) {
            phi = M_PI*n_inv*((float)k+0.5f)*(i+1);
            _p->yr[i] += _p->xr[k]*sinf(phi);
        }

        // compensate for discrepancy
        _p->yr[i] *= 2.0f;
    }
}

// DST-III
void FFT(_execute_RODFT01)(FFT(plan) _p)
{
    // ugly, slow method
    unsigned int i,k;
    float n_inv = 1.0f / (float)(_p->n);
    float phi;
    for (i=0; i<_p->n; i++) {
        _p->yr[i] = ((i%2)==0 ? 0.5f : -0.5f) * _p->xr[_p->n-1];
        for (k=0; k<_p->n-1; k++) {
            phi = M_PI*n_inv*((float)k+1)*((float)i+0.5f);
            _p->yr[i] += _p->xr[k]*sinf(phi);
        }

        // compensate for discrepancy
        _p->yr[i] *= 2.0f;
    }
}

// DST-IV
void FFT(_execute_RODFT11)(FFT(plan) _p)
{
    // ugly, slow method
    unsigned int i,k;
    float n_inv = 1.0f / (float)(_p->n);
    float phi;
    for (i=0; i<_p->n; i++) {
        _p->yr[i] = 0.0f;
        for (k=0; k<_p->n; k++) {
            phi = M_PI*n_inv*((float)k+0.5f)*((float)i+0.5f);
            _p->yr[i] += _p->xr[k]*sinf(phi);
        }

        // compensate for discrepancy
        _p->yr[i] *= 2.0f;
    }
}

