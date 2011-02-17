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

// DCT-I
void FFT(_execute_REDFT00)(FFT(plan) _p)
{
    // ugly, slow method
    unsigned int i,k;
    float n_inv = 1.0f / (float)(_p->n-1);
    float phi;
    for (i=0; i<_p->n; i++) {
        T x0 = _p->xr[0];       // first element
        T xn = _p->xr[_p->n-1]; // last element
        _p->yr[i] = 0.5f*( x0 + (i%2 ? -xn : xn));
        for (k=1; k<_p->n-1; k++) {
            phi = M_PI*n_inv*((float)k)*((float)i);
            _p->yr[i] += _p->xr[k]*cosf(phi);
        }

        // compensate for discrepancy
        _p->yr[i] *= 2.0f;
    }
}

// DCT-II (regular 'dct')
void FFT(_execute_REDFT10)(FFT(plan) _p)
{
#if 0
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

        // compensate for discrepancy
        _p->yr[i] *= 2.0f;
    }
#else
    unsigned int i;
    // precondition fft
    unsigned int r = _p->n % 2;
    unsigned int L = (_p->n - r)/2;

    for (i=0; i<L; i++) {
        _p->xc[i]         = _p->xr[2*i+0];
        _p->xc[_p->n-i-1] = _p->xr[2*i+1];
    }
    // check for odd condition
    if (r==1)
        _p->xc[L] = _p->xr[2*L];

    // execute fft, storing result in _p->yc
    FFT(_execute)(_p->internal_plan);

    // post-condition output
    for (i=0; i<_p->n; i++)
        _p->yr[i] = 2.0f*crealf(_p->yc[i]*cexpf(-_Complex_I*0.5f*M_PI*i/((float)_p->n)));
#endif
}

// DCT-III (regular 'idct')
void FFT(_execute_REDFT01)(FFT(plan) _p)
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

        // compensate for discrepancy
        _p->yr[i] *= 2.0f;
    }
}

// DCT-IV
void FFT(_execute_REDFT11)(FFT(plan) _p)
{
    // ugly, slow method
    unsigned int i,k;
    float n_inv = 1.0f / (float)(_p->n);
    float phi;
    for (i=0; i<_p->n; i++) {
        _p->yr[i] = 0.0f;
        for (k=0; k<_p->n; k++) {
            phi = M_PI*n_inv*((float)k+0.5f)*((float)i+0.5f);
            _p->yr[i] += _p->xr[k]*cosf(phi);
        }

        // compensate for discrepancy
        _p->yr[i] *= 2.0f;
    }
}
