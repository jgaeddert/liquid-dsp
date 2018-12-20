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
// Rational-rate resampler
//
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct RRESAMP(_s) {
    // filter design parameters
    unsigned int    P;      // interpolation factor
    unsigned int    Q;      // decimation factor
    unsigned int    m;      // filter semi-length, h_len = 2*m + 1
    float           As;     // filter stop-band attenuation
    float           bw;     // filter bandwidth

    // polyphase filterbank properties/object
    FIRPFB()        pfb;    // filterbank object (interpolator), Q filters in bank
};

// Create rational-rate resampler object from filter prototype
//  _P      : interpolation factor,                     P > 0
//  _Q      : decimation factor,                        Q > 0
//  _m      : filter semi-length (delay),               0 < _m
//  _bw     : filter bandwidth relative to sample rate, 0 < _bw= < 0.5
//  _As     : filter stop-band attenuation [dB],        0 < _As
RRESAMP() RRESAMP(_create)(unsigned int _P,
                           unsigned int _Q,
                           unsigned int _m,
                           float        _bw,
                           float        _As)
{
    // validate input
    if (_P == 0) {
        fprintf(stderr,"error: rresamp_%s_create(), interpolation rate must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    } else if (_Q == 0) {
        fprintf(stderr,"error: rresamp_%s_create(), decimation rate must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    } else if (_m == 0) {
        fprintf(stderr,"error: rresamp_%s_create(), filter semi-length must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    } else if (_bw <= 0.0f || _bw > 0.5f) {
        fprintf(stderr,"error: rresamp_%s_create(), filter bandwidth must be in (0,0.5]\n", EXTENSION_FULL);
        exit(1);
    } else if (_As <= 0.0f) {
        fprintf(stderr,"error: rresamp_%s_create(), filter stop-band suppression must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    }

    // allocate memory for resampler
    RRESAMP() q = (RRESAMP()) malloc(sizeof(struct RRESAMP(_s)));

    // set properties, scaling interpolation and decimation factors by their
    // greatest common divisor
    unsigned int gcd = liquid_gcd(_P, _Q);
    q->P  = _P / gcd;
    q->Q  = _Q / gcd;
    q->m  = _m;
    q->bw = _bw;
    q->As = _As;

    // design filter
    q->pfb = FIRPFB(_create_kaiser)(q->P,q->m,q->bw,q->As);
    RRESAMP(_set_scale)(q, 2.0f*q->bw*sqrtf((float)(q->Q)/(float)(q->P)));

    // reset object and return
    RRESAMP(_reset)(q);
    return q;
}

// create rational-rate resampler object with a specified input
// resampling rate and default parameters
//  m (filter semi-length) = 12
//  As (filter stop-band attenuation) = 60 dB
RRESAMP() RRESAMP(_create_default)(unsigned int _P,
                                   unsigned int _Q)
{
    // validate input
    if (_P == 0) {
        fprintf(stderr,"error: rresamp_%s_create(), interpolation rate must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    } else if (_Q == 0) {
        fprintf(stderr,"error: rresamp_%s_create(), decimation rate must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    }

    // det default parameters
    unsigned int m  = 12;
    float        bw = 0.5f;
    float        As = 60.0f;

    // create and return resamp object
    return RRESAMP(_create)(_P, _Q, m, bw, As);
}

// free resampler object
void RRESAMP(_destroy)(RRESAMP() _q)
{
    // free polyphase filterbank
    FIRPFB(_destroy)(_q->pfb);

    // free main object memory
    free(_q);
}

// print resampler object
void RRESAMP(_print)(RRESAMP() _q)
{
    printf("resampler [rate: %u/%u=%.6f], m=%u, bw=%.3f/Fs, As=%.3f dB\n",
            _q->P, _q->Q, (float)(_q->P) / (float)(_q->Q),
            _q->m, _q->bw, _q->As);
}

// reset resampler object
void RRESAMP(_reset)(RRESAMP() _q)
{
    // clear filterbank
    FIRPFB(_reset)(_q->pfb);
}

// Set output scaling for filter, default: \( 2 w \sqrt{P/Q} \)
//  _q      : resampler object
//  _scale  : scaling factor to apply to each output sample
void RRESAMP(_set_scale)(RRESAMP() _q,
                         TC        _scale)
{
    FIRPFB(_set_scale)(_q->pfb, _scale);
}

// Get output scaling for filter
//  _q      : resampler object
//  _scale  : scaling factor to apply to each output sample
void RRESAMP(_get_scale)(RRESAMP() _q,
                         TC *      _scale)
{
    FIRPFB(_get_scale)(_q->pfb, _scale);
}

// get resampler filter delay (semi-length m)
unsigned int RRESAMP(_get_delay)(RRESAMP() _q)
{
    return _q->m;
}

// Get rate of resampler, r = P/Q
float RRESAMP(_get_rate)(RRESAMP() _q)
{
    return (float)(_q->P) / (float)(_q->Q);
}

// Get interpolation factor of resampler, \(P\), after removing
// greatest common divisor
unsigned int RRESAMP(_get_interp)(RRESAMP() _q)
{
    return _q->P;
}

// Get decimator factor of resampler, \(Q\), after removing
// greatest common divisor
unsigned int RRESAMP(_get_decim)(RRESAMP() _q)
{
    return _q->Q;
}

// Execute rational-rate resampler on a block of input samples and
// store the resulting samples in the output array.
//  _q  : resamp object
//  _x  : input sample array, [size: Q x 1]
//  _y  : output sample array [size: P x 1]
void RRESAMP(_execute)(RRESAMP() _q,
                       TI *      _x,
                       TO *      _y)
{
    unsigned int index = 0; // filterbank index
    unsigned int i, n=0;
    for (i=0; i<_q->Q; i++) {
        // push input
        FIRPFB(_push)(_q->pfb, _x[i]);

        // continue to produce output
        while (index < _q->P) {
            FIRPFB(_execute)(_q->pfb, index, &_y[n++]);
            index += _q->Q;
        }

        // decrement filter-bank index by output rate
        index -= _q->P;
    }

    // error checking for now
    assert(index == 0);
    assert(n == _q->P);
}

