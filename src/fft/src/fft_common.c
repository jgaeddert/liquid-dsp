/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011, 2012 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2011, 2012 Virginia Polytechnic
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
// fft.common.c
//

#include <stdio.h>
#include <stdlib.h>
#include "liquid.internal.h"

struct FFT(plan_s) {
    unsigned int nfft;  // fft size
    TC * twiddle;       // twiddle factors
    TC * x;             // input array pointer (not allocated)
    TC * y;             // output array pointer (not allocated)
    int direction;      // forward/reverse
    int flags;
    liquid_fft_kind kind;
    liquid_fft_method method;

    // 'execute' function pointer
    void (*execute)(FFT(plan));

    // radix-2 transform data
    unsigned int m;             // log2(nfft)
    unsigned int * index_rev;   // reversed indices

    // real even/odd DFT parameters (DCT/DST)
    T * xr; // input array (real)
    T * yr; // output array (real)
};

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
    // determine best method for execution
    // TODO : check flags and allow user override
    liquid_fft_method method = liquid_fft_estimate_method(_nfft);

    // initialize fft based on method
    switch (method) {
    case LIQUID_FFT_METHOD_DFT:    return FFT(_create_plan_dft)(   _nfft, _x, _y, _dir, _flags);
    case LIQUID_FFT_METHOD_RADIX2: return FFT(_create_plan_radix2)(_nfft, _x, _y, _dir, _flags);
    case LIQUID_FFT_METHOD_UNKNOWN:
    default:
        fprintf(stderr,"error: fft_create_plan(), unknown/invalid fft method\n");
        exit(1);
    }

    return NULL;
}

// destroy FFT plan
void FFT(_destroy_plan)(FFT(plan) _q)
{
    switch (_q->method) {
    case LIQUID_FFT_METHOD_DFT:    FFT(_destroy_plan_dft)(_q);    break;
    case LIQUID_FFT_METHOD_RADIX2: FFT(_destroy_plan_radix2)(_q); break;
    case LIQUID_FFT_METHOD_UNKNOWN:
    default:
        fprintf(stderr,"error: fft_destroy_plan(), unknown/invalid fft method\n");
        exit(1);
    }
}

// execute fft
void FFT(_execute)(FFT(plan) _q)
{
    // invoke internal function pointer
    _q->execute(_q);
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

// perform _n-point fft shift
void FFT(_shift)(TC *_x, unsigned int _n)
{
    unsigned int i, n2;
    if (_n%2)
        n2 = (_n-1)/2;
    else
        n2 = _n/2;

    TC tmp;
    for (i=0; i<n2; i++) {
        tmp = _x[i];
        _x[i] = _x[i+n2];
        _x[i+n2] = tmp;
    }
}

