/*
 * Copyright (c) 2007 - 2018 Joseph Gaeddert
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
// Interpolator
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct FIRINTERP(_s) {
    TC *            h;          // prototype filter coefficients
    unsigned int    h_len;      // prototype filter length
    unsigned int    h_sub_len;  // sub-filter length
    unsigned int    M;          // interpolation factor
    FIRPFB()        filterbank; // polyphase filterbank object
};

// create interpolator
//  _M      :   interpolation factor
//  _h      :   filter coefficients array [size: _h_len x 1]
//  _h_len  :   filter length
FIRINTERP() FIRINTERP(_create)(unsigned int _M,
                               TC *         _h,
                               unsigned int _h_len)
{
    // validate input
    if (_M < 2) {
        fprintf(stderr,"error: firinterp_%s_create(), interp factor must be greater than 1\n", EXTENSION_FULL);
        exit(1);
    } else if (_h_len < _M) {
        fprintf(stderr,"error: firinterp_%s_create(), filter length cannot be less than interp factor\n", EXTENSION_FULL);
        exit(1);
    }

    // allocate main object memory and set internal parameters
    FIRINTERP() q = (FIRINTERP()) malloc(sizeof(struct FIRINTERP(_s)));
    q->M = _M;
    q->h_len = _h_len;

    // compute sub-filter length
    q->h_sub_len=0;
    while (q->M * q->h_sub_len < _h_len)
        q->h_sub_len++;

    // compute effective filter length (pad end of prototype with zeros)
    q->h_len = q->M * q->h_sub_len;
    q->h = (TC*) malloc((q->h_len)*sizeof(TC));

    // load filter coefficients in regular order, padding end with zeros
    unsigned int i;
    for (i=0; i<q->h_len; i++)
        q->h[i] = i < _h_len ? _h[i] : 0.0f;

    // create polyphase filterbank
    q->filterbank = FIRPFB(_create)(q->M, q->h, q->h_len);

    // return interpolator object
    return q;
}

// create interpolator from Kaiser prototype
//  _M      :   interpolation factor
//  _m      :   symbol delay
//  _As     :   stop-band attenuation [dB]
FIRINTERP() FIRINTERP(_create_kaiser)(unsigned int _M,
                                      unsigned int _m,
                                      float        _As)
{
    // validate input
    if (_M < 2) {
        fprintf(stderr,"error: firinterp_%s_create_kaiser(), interp factor must be greater than 1\n", EXTENSION_FULL);
        exit(1);
    } else if (_m == 0) {
        fprintf(stderr,"error: firinterp_%s_create_kaiser(), filter delay must be greater than 0\n", EXTENSION_FULL);
        exit(1);
    } else if (_As < 0.0f) {
        fprintf(stderr,"error: firinterp_%s_create_kaiser(), stop-band attenuation must be positive\n", EXTENSION_FULL);
        exit(1);
    }

    // compute filter coefficients (floating point precision)
    unsigned int h_len = 2*_M*_m + 1;
    float hf[h_len];
    float fc = 0.5f / (float) (_M);
    liquid_firdes_kaiser(h_len, fc, _As, 0.0f, hf);

    // copy coefficients to type-specific array (e.g. float complex)
    TC hc[h_len];
    unsigned int i;
    for (i=0; i<h_len; i++)
        hc[i] = hf[i];
    
    // return interpolator object
    return FIRINTERP(_create)(_M, hc, 2*_M*_m);
}

// create prototype (root-)Nyquist interpolator
//  _type   :   filter type (e.g. LIQUID_NYQUIST_RCOS)
//  _k      :   samples/symbol,          _k > 1
//  _m      :   filter delay (symbols),  _m > 0
//  _beta   :   excess bandwidth factor, _beta < 1
//  _dt     :   fractional sample delay, _dt in (-1, 1)
FIRINTERP() FIRINTERP(_create_prototype)(int          _type,
                                         unsigned int _k,
                                         unsigned int _m,
                                         float        _beta,
                                         float        _dt)
{
    // validate input
    if (_k < 2) {
        fprintf(stderr,"error: firinterp_%s_create_prototype(), interp factor must be greater than 1\n", EXTENSION_FULL);
        exit(1);
    } else if (_m == 0) {
        fprintf(stderr,"error: firinterp_%s_create_prototype(), filter delay must be greater than 0\n", EXTENSION_FULL);
        exit(1);
    } else if (_beta < 0.0f || _beta > 1.0f) {
        fprintf(stderr,"error: firinterp_%s_create_prototype(), filter excess bandwidth factor must be in [0,1]\n", EXTENSION_FULL);
        exit(1);
    } else if (_dt < -1.0f || _dt > 1.0f) {
        fprintf(stderr,"error: firinterp_%s_create_prototype(), filter fractional sample delay must be in [-1,1]\n", EXTENSION_FULL);
        exit(1);
    }

    // generate Nyquist filter
    unsigned int h_len = 2*_k*_m + 1;
    float h[h_len];
    liquid_firdes_prototype(_type,_k,_m,_beta,_dt,h);

    // copy coefficients to type-specific array (e.g. float complex)
    unsigned int i;
    TC hc[h_len];
    for (i=0; i<h_len; i++)
        hc[i] = h[i];

    // return interpolator object
    return FIRINTERP(_create)(_k, hc, h_len);
}

// destroy interpolator object
void FIRINTERP(_destroy)(FIRINTERP() _q)
{
    FIRPFB(_destroy)(_q->filterbank);
    free(_q->h);
    free(_q);
}

// print interpolator state
void FIRINTERP(_print)(FIRINTERP() _q)
{
    printf("interp():\n");
    printf("    M       :   %u\n", _q->M);
    printf("    h_len   :   %u\n", _q->h_len);
    FIRPFB(_print)(_q->filterbank);
}

// clear internal state
void FIRINTERP(_reset)(FIRINTERP() _q)
{
    FIRPFB(_reset)(_q->filterbank);
}

// Set output scaling for interpolator
//  _q      : interpolator object
//  _scale  : scaling factor to apply to each output sample
void FIRINTERP(_set_scale)(FIRINTERP() _q,
                           TC          _scale)
{
    FIRPFB(_set_scale)(_q->filterbank, _scale);
}

// Get output scaling for interpolator
//  _q      : interpolator object
//  _scale  : scaling factor to apply to each output sample
void FIRINTERP(_get_scale)(FIRINTERP() _q,
                           TC *        _scale)
{
    FIRPFB(_get_scale)(_q->filterbank, _scale);
}

// execute interpolator
//  _q      : interpolator object
//  _x      : input sample
//  _y      : output array [size: 1 x _M]
void FIRINTERP(_execute)(FIRINTERP() _q,
                         TI          _x,
                         TO *        _y)
{
    // push sample into filterbank
    FIRPFB(_push)(_q->filterbank,  _x);

    // compute output for each filter in the bank
    unsigned int i;
    for (i=0; i<_q->M; i++)
        FIRPFB(_execute)(_q->filterbank, i, &_y[i]);
}

// execute interpolation on block of input samples
//  _q      : firinterp object
//  _x      : input array [size: _n x 1]
//  _n      : size of input array
//  _y      : output sample array [size: _M*_n x 1]
void FIRINTERP(_execute_block)(FIRINTERP()  _q,
                               TI *         _x,
                               unsigned int _n,
                               TO *         _y)
{
    unsigned int i;
    for (i=0; i<_n; i++) {
        // execute one input at a time with an output stride _M
        FIRINTERP(_execute)(_q, _x[i], &_y[i*_q->M]);
    }
}

