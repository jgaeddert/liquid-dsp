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
// fft_r2r_1d.c : real-to-real methods (DCT/DST)
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "liquid.internal.h"

// create DCT/DST plan
//  _nfft   :   FFT size
//  _x      :   input array [size: _nfft x 1]
//  _y      :   output array [size: _nfft x 1]
//  _type   :   type (e.g. LIQUID_FFT_REDFT00)
//  _method :   fft method
FFT(plan) FFT(_create_plan_r2r_1d)(unsigned int _nfft,
                                   T *          _x,
                                   T *          _y,
                                   int          _type,
                                   int          _flags)
{
    // allocate plan and initialize all internal arrays to NULL
    FFT(plan) q = (FFT(plan)) malloc(sizeof(struct FFT(plan_s)));

    q->nfft   = _nfft;
    q->xr     = _x;
    q->yr     = _y;
    q->type   = _type;
    q->flags  = _flags;

    // TODO : use separate 'method' for real-to-real types
    //q->method = LIQUID_FFT_METHOD_NONE;

    switch (q->type) {
    case LIQUID_FFT_REDFT00:  q->execute = &FFT(_execute_REDFT00);  break;  // DCT-I
    case LIQUID_FFT_REDFT10:  q->execute = &FFT(_execute_REDFT10);  break;  // DCT-II
    case LIQUID_FFT_REDFT01:  q->execute = &FFT(_execute_REDFT01);  break;  // DCT-III
    case LIQUID_FFT_REDFT11:  q->execute = &FFT(_execute_REDFT11);  break;  // DCT-IV

    case LIQUID_FFT_RODFT00:  q->execute = &FFT(_execute_RODFT00);  break;  // DST-I
    case LIQUID_FFT_RODFT10:  q->execute = &FFT(_execute_RODFT10);  break;  // DST-II
    case LIQUID_FFT_RODFT01:  q->execute = &FFT(_execute_RODFT01);  break;  // DST-III
    case LIQUID_FFT_RODFT11:  q->execute = &FFT(_execute_RODFT11);  break;  // DST-IV
    default:
        fprintf(stderr,"error: fft_create_plan_r2r_1d(), invalid type, %d\n", q->type);
        exit(1);
    }

    return q;
}

// destroy real-to-real transform plan
void FFT(_destroy_plan_r2r_1d)(FFT(plan) _q)
{
    // free main object memory
    free(_q);
}

// print real-to-real transform plan
void FFT(_print_plan_r2r_1d)(FFT(plan) _q)
{
    printf("real-to-real transform...\n");
    // TODO: print actual transform type
}

//
// DCT : Discrete Cosine Transforms
//

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

//
// DST : Discrete Sine Transforms
//

// DST-I
void FFT(_execute_RODFT00)(FFT(plan) _q)
{
    // ugly, slow method
    unsigned int i,k;
    float n_inv = 1.0f / (float)(_q->nfft+1);
    float phi;
    for (i=0; i<_q->nfft; i++) {
        _q->yr[i] = 0.0f;
        for (k=0; k<_q->nfft; k++) {
            phi = M_PI*n_inv*(float)((k+1)*(i+1));
            _q->yr[i] += _q->xr[k]*sinf(phi);
        }

        // compensate for discrepancy
        _q->yr[i] *= 2.0f;
    }
}

// DST-II
void FFT(_execute_RODFT10)(FFT(plan) _q)
{
    // ugly, slow method
    unsigned int i,k;
    float n_inv = 1.0f / (float)(_q->nfft);
    float phi;
    for (i=0; i<_q->nfft; i++) {
        _q->yr[i] = 0.0f;
        for (k=0; k<_q->nfft; k++) {
            phi = M_PI*n_inv*((float)k+0.5f)*(i+1);
            _q->yr[i] += _q->xr[k]*sinf(phi);
        }

        // compensate for discrepancy
        _q->yr[i] *= 2.0f;
    }
}

// DST-III
void FFT(_execute_RODFT01)(FFT(plan) _q)
{
    // ugly, slow method
    unsigned int i,k;
    float n_inv = 1.0f / (float)(_q->nfft);
    float phi;
    for (i=0; i<_q->nfft; i++) {
        _q->yr[i] = ((i%2)==0 ? 0.5f : -0.5f) * _q->xr[_q->nfft-1];
        for (k=0; k<_q->nfft-1; k++) {
            phi = M_PI*n_inv*((float)k+1)*((float)i+0.5f);
            _q->yr[i] += _q->xr[k]*sinf(phi);
        }

        // compensate for discrepancy
        _q->yr[i] *= 2.0f;
    }
}

// DST-IV
void FFT(_execute_RODFT11)(FFT(plan) _q)
{
    // ugly, slow method
    unsigned int i,k;
    float n_inv = 1.0f / (float)(_q->nfft);
    float phi;
    for (i=0; i<_q->nfft; i++) {
        _q->yr[i] = 0.0f;
        for (k=0; k<_q->nfft; k++) {
            phi = M_PI*n_inv*((float)k+0.5f)*((float)i+0.5f);
            _q->yr[i] += _q->xr[k]*sinf(phi);
        }

        // compensate for discrepancy
        _q->yr[i] *= 2.0f;
    }
}

