/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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

// create decimator with default Butterworth prototype
//  _M      : decimation factor
//  _order  : filter order
IIRINTERP() IIRINTERP(_create_default)(unsigned int _M,
                                       unsigned int _order)
{
    return IIRINTERP(_create_prototype)(_M,
                                        LIQUID_IIRDES_BUTTER,
                                        LIQUID_IIRDES_LOWPASS,
                                        LIQUID_IIRDES_SOS,
                                        _order,
                                        0.5f / (float)_M,    // fc
                                        0.0f,                // f0
                                        0.1f,                // pass-band ripple,
                                        60.0f);              // stop-band attenuation
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

