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
// firdecim.c
//
// finite impulse response decimator object definitions
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// decimator structure
struct IIRDECIM(_s) {
    unsigned int M;     // decimation factor

    // TODO: use IIR polyphase filterbank
    IIRFILT() iirfilt;  // filter object
};

// create interpolator from external coefficients
//  _M      : interpolation factor
//  _b      : feed-back coefficients [size: _nb x 1]
//  _nb     : feed-back coefficients length
//  _a      : feed-forward coefficients [size: _na x 1]
//  _na     : feed-forward coefficients length
IIRDECIM() IIRDECIM(_create)(unsigned int _M,
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
    IIRDECIM() q = (IIRDECIM()) malloc(sizeof(struct IIRDECIM(_s)));
    q->M = _M;

    // create filter
    q->iirfilt = IIRFILT(_create)(_b, _nb, _a, _na);

    // return interpolator object
    return q;
}

// create interpolator from prototype
//  _M      :   interpolation factor
IIRDECIM() IIRDECIM(_create_prototype)(unsigned int _M,
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
    IIRDECIM() q = (IIRDECIM()) malloc(sizeof(struct IIRDECIM(_s)));
    q->M = _M;

    // create filter
    q->iirfilt = IIRFILT(_create_prototype)(_ftype, _btype, _format, _order, _fc, _f0, _Ap, _As);

    // return interpolator object
    return q;
}

// destroy interpolator object
void IIRDECIM(_destroy)(IIRDECIM() _q)
{
    IIRFILT(_destroy)(_q->iirfilt);
    free(_q);
}

// print interpolator state
void IIRDECIM(_print)(IIRDECIM() _q)
{
    printf("interp():\n");
    printf("    M       :   %u\n", _q->M);
    IIRFILT(_print)(_q->iirfilt);
}

// clear internal state
void IIRDECIM(_reset)(IIRDECIM() _q)
{
    IIRFILT(_reset)(_q->iirfilt);
}

// execute decimator
//  _q      :   decimator object
//  _x      :   input sample array [size: _M x 1]
//  _y      :   output sample pointer
//  _index  :   decimator output index [0,_M-1]
void IIRDECIM(_execute)(IIRDECIM()   _q,
                        TI *         _x,
                        TO *         _y)
{
    TO v; // output value
    unsigned int i;
    for (i=0; i<_q->M; i++) {
        // run filter
        IIRFILT(_execute)(_q->iirfilt, _x[i], &v);

        // save output at appropriate index
        if (i==0)
            *_y = v;
    }
}

// execute decimator on block of _n*_M input samples
//  _q      : decimator object
//  _x      : input array [size: _n*_M x 1]
//  _n      : number of _output_ samples
//  _y      : output array [_sze: _n x 1]
void IIRDECIM(_execute_block)(IIRDECIM()   _q,
                              TI *         _x,
                              unsigned int _n,
                              TO *         _y)
{
    unsigned int i;
    for (i=0; i<_n; i++) {
        // execute _M input samples computing just one output each time
        IIRDECIM(_execute)(_q, &_x[i*_q->M], &_y[i]);
    }
}

// get system group delay at frequency _fc
//  _q      :   interpolator object
//  _f      :   frequency
float IIRDECIM(_groupdelay)(IIRDECIM() _q,
                            float      _fc)
{
    return IIRFILT(_groupdelay)(_q->iirfilt, _fc);
}

