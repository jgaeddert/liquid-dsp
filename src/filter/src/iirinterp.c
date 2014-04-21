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
// infinite impulse response interpolator
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct IIRINTERP(_s) {
    unsigned int M;     // interpolation factor

    // TODO: use IIR polyphase filterbank
    IIRFILT() iirfilt;  // filter object

};

// create interpolator from external coefficients
//  _M      : interpolation factor
//  _b      : feed-back coefficients [size: _nb x 1]
//  _nb     : feed-back coefficients length
//  _a      : feed-forward coefficients [size: _na x 1]
//  _na     : feed-forward coefficients length
IIRINTERP() IIRINTERP(_create)(unsigned int _M,
                               TC *         _b,
                               unsigned int _nb,
                               TC *         _a,
                               unsigned int _na)
{
    // validate input
    if (_M < 2) {
        fprintf(stderr,"error: iirinterp_%s_create(), interp factor must be greater than 1\n", EXTENSION_FULL);
        exit(1);
    }

    // allocate main object memory and set internal parameters
    IIRINTERP() q = (IIRINTERP()) malloc(sizeof(struct IIRINTERP(_s)));
    q->M = _M;

    // create filter
    q->iirfilt = IIRFILT(_create)(_b, _nb, _a, _na);

    // return interpolator object
    return q;
}

// create interpolator from prototype
//  _M      :   interpolation factor
IIRINTERP() IIRINTERP(_create_prototype)(unsigned int _M,
                                         liquid_iirdes_filtertype _ftype,
                                         liquid_iirdes_bandtype   _btype,
                                         liquid_iirdes_format     _format,
                                         unsigned int _order,
                                         float _fc,
                                         float _f0,
                                         float _Ap,
                                         float _As)
{
    // validate input
    if (_M < 2) {
        fprintf(stderr,"error: iirinterp_%s_create_prototype(), interp factor must be greater than 1\n", EXTENSION_FULL);
        exit(1);
    }

    // allocate main object memory and set internal parameters
    IIRINTERP() q = (IIRINTERP()) malloc(sizeof(struct IIRINTERP(_s)));
    q->M = _M;

    // create filter
    q->iirfilt = IIRFILT(_create_prototype)(_ftype, _btype, _format, _order, _fc, _f0, _Ap, _As);

    // return interpolator object
    return q;
}

// destroy interpolator object
void IIRINTERP(_destroy)(IIRINTERP() _q)
{
    IIRFILT(_destroy)(_q->iirfilt);
    free(_q);
}

// print interpolator state
void IIRINTERP(_print)(IIRINTERP() _q)
{
    printf("interp():\n");
    printf("    M       :   %u\n", _q->M);
    IIRFILT(_print)(_q->iirfilt);
}

// clear internal state
void IIRINTERP(_reset)(IIRINTERP() _q)
{
    IIRFILT(_reset)(_q->iirfilt);
}

// execute interpolator
//  _q      :   interpolator object
//  _x      :   input sample
//  _y      :   output array [size: 1 x _M]
void IIRINTERP(_execute)(IIRINTERP() _q,
                         TI          _x,
                         TO *        _y)
{
    // TODO: use iirpfb
    unsigned int i;
    for (i=0; i<_q->M; i++)
        IIRFILT(_execute)(_q->iirfilt, i==0 ? _x : 0.0f, &_y[i]);
}

// execute interpolation on block of input samples
//  _q      : iirinterp object
//  _x      : input array [size: _n x 1]
//  _n      : size of input array
//  _y      : output sample array [size: _M*_n x 1]
void IIRINTERP(_execute_block)(IIRINTERP()  _q,
                               TI *         _x,
                               unsigned int _n,
                               TO *         _y)
{
    unsigned int i;
    for (i=0; i<_n; i++) {
        // execute one input at a time with an output stride _M
        IIRINTERP(_execute)(_q, _x[i], &_y[i*_q->M]);
    }
}

// get system group delay at frequency _fc
//  _q      :   interpolator object
//  _f      :   frequency
float IIRINTERP(_groupdelay)(IIRINTERP() _q,
                             float       _fc)
{
    return IIRFILT(_groupdelay)(_q->iirfilt, _fc) / (float) (_q->M);
}

