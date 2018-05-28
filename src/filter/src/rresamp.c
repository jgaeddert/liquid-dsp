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
//  _P      : interpolation factor,              P > 0
//  _Q      : decimation factor,                 Q > 0
//  _m      : filter semi-length (delay),        0 < _m
//  _As     : filter stop-band attenuation [dB], 0 < _As
RRESAMP() RRESAMP(_create)(unsigned int _P,
                           unsigned int _Q,
                           unsigned int _m,
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
    } else if (_As <= 0.0f) {
        fprintf(stderr,"error: rresamp_%s_create(), filter stop-band suppression must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    }

    // TODO: reduce P and Q by gcd(P,Q)

    // allocate memory for resampler
    RRESAMP() q = (RRESAMP()) malloc(sizeof(struct RRESAMP(_s)));

    // set properties
    q->P    = _P;
    q->Q    = _Q;
    q->m    = _m;
    q->As   = _As;

    // design filter
    q->bw = 0.40f;
    q->pfb = FIRPFB(_create_kaiser)(q->Q,q->m,q->bw,q->As);
    FIRPFB(_set_scale)(q->pfb, 2.0f*q->bw*sqrtf((float)(q->P)/(float)(q->Q)));

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
    float        As = 60.0f;

    // create and return resamp object
    return RRESAMP(_create)(_P, _Q, m, As);
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
    float rate = (float)(_q->P) / (float)(_q->Q);
    printf("resampler [rate: %u/%u=%f]\n", _q->P, _q->Q, rate);
    FIRPFB(_print)(_q->pfb);
}

// reset resampler object
void RRESAMP(_reset)(RRESAMP() _q)
{
    // clear filterbank
    FIRPFB(_reset)(_q->pfb);
}

// get resampler filter delay (semi-length m)
unsigned int RRESAMP(_get_delay)(RRESAMP() _q)
{
    return _q->m;
}

// Get rate of resampler, r = P/Q
float RRESAMP(_get_rate)(RRESAMP() _q)
{
    return (float)(_q->Q) / (float)(_q->P);
}

// Execute rational-rate resampler on a block of input samples and
// store the resulting samples in the output array.
//  _q  : resamp object
//  _x  : input sample array, [size: P x 1]
//  _y  : output sample array [size: Q x 1]
void RRESAMP(_execute)(RRESAMP() _q,
                       TI *      _x,
                       TO *      _y)
{
    unsigned int index = 0; // filterbank index
    unsigned int i, n=0;
    for (i=0; i<_q->P; i++) {
        // push input
        FIRPFB(_push)(_q->pfb, _x[i]);

        // continue to produce output
        while (index < _q->Q) {
            FIRPFB(_execute)(_q->pfb, index, &_y[n++]);
            index += _q->P;
        }

        // decrement filter-bank index by output rate
        index -= _q->Q;
    }

    // error checking for now
    assert(index == 0);
    assert(n == _q->Q);
}

