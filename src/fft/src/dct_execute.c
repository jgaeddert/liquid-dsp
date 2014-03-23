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
// DCT : Discrete Cosine Transforms
//

#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

// DCT-I
void FFT(_execute_REDFT00)(FFT(plan) _q)
{
    // ugly, slow method
    unsigned int i,k;
    float n_inv = 1.0f / (float)(_q->nfft-1);
    float phi;
    for (i=0; i<_q->nfft; i++) {
        T x0 = _q->xr[0];       // first element
        T xn = _q->xr[_q->nfft-1]; // last element
        _q->yr[i] = 0.5f*( x0 + (i%2 ? -xn : xn));
        for (k=1; k<_q->nfft-1; k++) {
            phi = M_PI*n_inv*((float)k)*((float)i);
            _q->yr[i] += _q->xr[k]*cosf(phi);
        }

        // compensate for discrepancy
        _q->yr[i] *= 2.0f;
    }
}

// DCT-II (regular 'dct')
void FFT(_execute_REDFT10)(FFT(plan) _q)
{
    // ugly, slow method
    unsigned int i,k;
    float n_inv = 1.0f / (float)_q->nfft;
    float phi;
    for (i=0; i<_q->nfft; i++) {
        _q->yr[i] = 0.0f;
        for (k=0; k<_q->nfft; k++) {
            phi = M_PI*n_inv*((float)k + 0.5f)*i;
            _q->yr[i] += _q->xr[k]*cosf(phi);
        }

        // compensate for discrepancy
        _q->yr[i] *= 2.0f;
    }
}

// DCT-III (regular 'idct')
void FFT(_execute_REDFT01)(FFT(plan) _q)
{
    // ugly, slow method
    unsigned int i,k;
    float n_inv = 1.0f / (float)_q->nfft;
    float phi;
    for (i=0; i<_q->nfft; i++) {
        _q->yr[i] = _q->xr[0]*0.5f;
        for (k=1; k<_q->nfft; k++) {
            phi = M_PI*n_inv*((float)i + 0.5f)*k;
            _q->yr[i] += _q->xr[k]*cosf(phi);
        }

        // compensate for discrepancy
        _q->yr[i] *= 2.0f;
    }
}

// DCT-IV
void FFT(_execute_REDFT11)(FFT(plan) _q)
{
    // ugly, slow method
    unsigned int i,k;
    float n_inv = 1.0f / (float)(_q->nfft);
    float phi;
    for (i=0; i<_q->nfft; i++) {
        _q->yr[i] = 0.0f;
        for (k=0; k<_q->nfft; k++) {
            phi = M_PI*n_inv*((float)k+0.5f)*((float)i+0.5f);
            _q->yr[i] += _q->xr[k]*cosf(phi);
        }

        // compensate for discrepancy
        _q->yr[i] *= 2.0f;
    }
}
