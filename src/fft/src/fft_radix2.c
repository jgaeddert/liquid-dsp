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
// fft_radix2.c : definitions for transforms of the form 2^m
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
FFT(plan) FFT(_create_plan_radix2)(unsigned int _nfft,
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
    q->method    = LIQUID_FFT_METHOD_RADIX2;

    q->execute   = FFT(_execute_radix2);

    // initialize twiddle factors, indices for radix-2 transforms
    q->data.radix2.m = liquid_msb_index(q->nfft) - 1;  // m = log2(nfft)
    
    q->data.radix2.index_rev = (unsigned int *) malloc((q->nfft)*sizeof(unsigned int));
    unsigned int i;
    for (i=0; i<q->nfft; i++)
        q->data.radix2.index_rev[i] = fft_reverse_index(i,q->data.radix2.m);

    // initialize twiddle factors
    q->data.radix2.twiddle = (TC *) malloc(q->nfft * sizeof(TC));
    
    T d = (q->direction == FFT_FORWARD) ? -1.0 : 1.0;
    for (i=0; i<q->nfft; i++)
        q->data.radix2.twiddle[i] = cexpf(_Complex_I*d*2*M_PI*(T)i / (T)(q->nfft));

    return q;
}

// destroy FFT plan
void FFT(_destroy_plan_radix2)(FFT(plan) _q)
{
    // free data specific to radix-2 transforms
    free(_q->data.radix2.index_rev);
    free(_q->data.radix2.twiddle);

    // free main object memory
    free(_q);
}

// execute radix-2 FFT
void FFT(_execute_radix2)(FFT(plan) _q)
{
    // swap values
    unsigned int i,j,k;
    for (i=0; i<_q->nfft; i++)
        _q->y[i] = _q->x[ _q->data.radix2.index_rev[i] ];

    TC yp;
    TC *y=_q->y;
    unsigned int n1 = 0;
    unsigned int n2 = 1;

    TC t;   // twiddle value
    unsigned int stride = _q->nfft;
    unsigned int twiddle_index;

    for (i=0; i<_q->data.radix2.m; i++) {
        n1 = n2;
        n2 *= 2;
        stride >>= 1;

        twiddle_index = 0;
    
        for (j=0; j<n1; j++) {
            t = _q->data.radix2.twiddle[twiddle_index];
            twiddle_index = (twiddle_index + stride) % _q->nfft;

            for (k=j; k<_q->nfft; k+=n2) {
                yp      =  y[k+n1]*t;
                y[k+n1] =  y[k] - yp;
                y[k]    += yp;
            }
        }
    }
}

