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
// fft.c
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "liquid.internal.h"

// create FFT plan
//  _nfft   :   FFT size
//  _x      :   input array [size: _nfft x 1]
//  _y      :   output array [size: _nfft x 1]
//  _dir    :   fft direction: {FFT_FORWARD, FFT_REVERSE}
//  _method :   fft method
FFT(plan) FFT(_create_plan)(unsigned int _nfft,
                            TC *         _x,
                            TC *         _y,
                            int          _dir,
                            int          _flags)
{
    // allocate plan and initialize all internal arrays to NULL
    FFT(plan) q = (FFT(plan)) malloc(sizeof(struct FFT(plan_s)));

    q->nfft      = _nfft;
    q->x         = _x;
    q->y         = _y;
    q->flags     = _flags;
    q->kind      = LIQUID_FFT_DFT_1D;
    q->direction = (_dir == FFT_FORWARD) ? FFT_FORWARD : FFT_REVERSE;

    // determine best method for execution
    // TODO : check flags and allow user override
    q->method = liquid_fft_estimate_method(q->nfft);

    // initialize fft based on method (just DFT for now)
    switch (q->method) {
    case LIQUID_FFT_METHOD_UNKNOWN:
        fprintf(stderr,"error: fft_create_plan(), unknown/invalid fft method\n");
        exit(1);
    case LIQUID_FFT_METHOD_DFT:
    default:
        q->execute = FFT(_execute_dft);
    }

    return q;
}

// destroy FFT plan
void FFT(_destroy_plan)(FFT(plan) _p)
{
    // free main object memory
    free(_p);
}

// perform n-point FFT allocating plan internally
//  _nfft   :   fft size
//  _x      :   input array [size: _nfft x 1]
//  _y      :   output array [size: _nfft x 1]
//  _dir    :   fft direction: {FFT_FORWARD, FFT_REVERSE}
//  _method :   fft method
void FFT(_run)(unsigned int _nfft,
               TC *         _x,
               TC *         _y,
               int          _dir,
               int          _method)
{
    // create plan
    FFT(plan) plan = FFT(_create_plan)(_nfft, _x, _y, _dir, _method);

    // execute fft
    FFT(_execute)(plan);

    // destroy plan
    FFT(_destroy_plan)(plan);
}

// execute DFT (slow but accurate)
void FFT(_execute_dft)(FFT(plan) _q)
{
    unsigned int i;
    unsigned int k;
    unsigned int nfft = _q->nfft;
    
    T d = (_q->direction==FFT_FORWARD) ? -1 : 1;

    for (i=0; i<nfft; i++) {
        _q->y[i] = 0.0f;
        for (k=0; k<nfft; k++) {
            T phi = 2*M_PI*d*i*k / (float)nfft;
            _q->y[i] += _q->x[k] * cexpf(_Complex_I*phi);
        }
    }
}


// 
// real-to-real methods
//


// create DCT/DST plan
//  _nfft   :   FFT size
//  _x      :   input array [size: _nfft x 1]
//  _y      :   output array [size: _nfft x 1]
//  _kind   :   type (e.g. FFT_REDFT00)
//  _method :   fft method
FFT(plan) FFT(_create_plan_r2r_1d)(unsigned int _nfft,
                                   T *          _x,
                                   T *          _y,
                                   int          _kind,
                                   int          _flags)
{
    // allocate plan and initialize all internal arrays to NULL
    FFT(plan) q = (FFT(plan)) malloc(sizeof(struct FFT(plan_s)));

    q->nfft   = _nfft;
    q->xr     = _x;
    q->yr     = _y;
    q->kind   = _kind;
    q->flags  = _flags;
    //q->method = LIQUID_FFT_METHOD_DFT;

    switch (q->kind) {
    case FFT_REDFT00:  q->execute = &FFT(_execute_REDFT00);  break;  // DCT-I
    case FFT_REDFT10:  q->execute = &FFT(_execute_REDFT10);  break;  // DCT-II
    case FFT_REDFT01:  q->execute = &FFT(_execute_REDFT01);  break;  // DCT-III
    case FFT_REDFT11:  q->execute = &FFT(_execute_REDFT11);  break;  // DCT-IV

    case FFT_RODFT00:  q->execute = &FFT(_execute_RODFT00);  break;  // DST-I
    case FFT_RODFT10:  q->execute = &FFT(_execute_RODFT10);  break;  // DST-II
    case FFT_RODFT01:  q->execute = &FFT(_execute_RODFT01);  break;  // DST-III
    case FFT_RODFT11:  q->execute = &FFT(_execute_RODFT11);  break;  // DST-IV
    default:
        fprintf(stderr,"error: fft_create_plan_r2r_1d(), invalid kind, %d\n", q->kind);
        exit(1);
    }

    return q;
}

