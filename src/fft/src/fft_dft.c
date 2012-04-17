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
// fft_dft.c : definitions for regular, slow DFTs
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "liquid.internal.h"

// create FFT plan for regular DFT
//  _nfft   :   FFT size
//  _x      :   input array [size: _nfft x 1]
//  _y      :   output array [size: _nfft x 1]
//  _dir    :   fft direction: {FFT_FORWARD, FFT_REVERSE}
//  _method :   fft method
FFT(plan) FFT(_create_plan_dft)(unsigned int _nfft,
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
    q->method    = LIQUID_FFT_METHOD_DFT;

    q->execute   = FFT(_execute_dft);

    // initialize twiddle factors
    q->twiddle = (TC *) malloc(q->nfft * sizeof(TC));
    
    unsigned int i;
    T d = (q->direction == FFT_FORWARD) ? -1.0 : 1.0;
    for (i=0; i<q->nfft; i++)
        q->twiddle[i] = cexpf(_Complex_I*d*2*M_PI*(T)i / (T)(q->nfft));

    return q;
}

// destroy FFT plan
void FFT(_destroy_plan_dft)(FFT(plan) _q)
{
    // free twiddle factors
    free(_q->twiddle);

    // free main object memory
    free(_q);
}

// execute DFT (slow but functionally correct)
void FFT(_execute_dft)(FFT(plan) _q)
{
    unsigned int i;
    unsigned int k;
    unsigned int nfft = _q->nfft;
    
    for (i=0; i<nfft; i++) {
        _q->y[i] = 0.0f;
        for (k=0; k<nfft; k++) {
            _q->y[i] += _q->x[k] * _q->twiddle[(i*k)%_q->nfft];
        }
    }
}

