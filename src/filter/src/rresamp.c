/*
 * Copyright (c) 2007 - 2019 Joseph Gaeddert
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
    unsigned int    gcd;    // greatest common divisor between inputs P and Q
    unsigned int    P;      // interpolation factor
    unsigned int    Q;      // decimation factor
    unsigned int    m;      // filter semi-length, h_len = 2*m + 1

    // polyphase filterbank properties/object
    FIRPFB()        pfb;    // filterbank object (interpolator), Q filters in bank
};

// internal: execute rational-rate resampler on a primitive-length block of
// input samples and store the resulting samples in the output array.
void RRESAMP(_execute_primitive)(RRESAMP() _q,
                                 TI *      _x,
                                 TO *      _y);

// Create rational-rate resampler object from external coefficients
//  _P      : interpolation factor,                     P > 0
//  _Q      : decimation factor,                        Q > 0
//  _m      : filter semi-length (delay),               0 < _m
//  _h      : filter coefficients, [size: 2*_P*_m x 1], 0 < _bw= < 0.5
RRESAMP() RRESAMP(_create)(unsigned int _P,
                           unsigned int _Q,
                           unsigned int _m,
                           TC *         _h)
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
    }

    // allocate memory for resampler
    RRESAMP() q = (RRESAMP()) malloc(sizeof(struct RRESAMP(_s)));

    // set properties, scaling interpolation and decimation factors by their
    // greatest common divisor
    q->gcd= liquid_gcd(_P, _Q);
    q->P  = _P / q->gcd;
    q->Q  = _Q / q->gcd;
    q->m  = _m;

    // create poly-phase filter bank
    q->pfb = FIRPFB(_create)(q->P, _h, 2*q->P*q->m);

    // reset object and return
    RRESAMP(_reset)(q);
    return q;
}

// Create rational-rate resampler object from filter prototype
//  _P      : interpolation factor,                     P > 0
//  _Q      : decimation factor,                        Q > 0
//  _m      : filter semi-length (delay),               0 < _m
//  _bw     : filter bandwidth relative to sample rate, 0 < _bw= < 0.5
//  _As     : filter stop-band attenuation [dB],        0 < _As
RRESAMP() RRESAMP(_create_kaiser)(unsigned int _P,
                                  unsigned int _Q,
                                  unsigned int _m,
                                  float        _bw,
                                  float        _As)
{
    // design filter
    unsigned int h_len = 2*_P*_m + 1;
    float * hf = (float*) malloc(h_len*sizeof(float));
    TC    * h  = (TC*)    malloc(h_len*sizeof(TC)   );
    liquid_firdes_kaiser(h_len, _bw/(float)_P, _As, 0.0f, hf);

    // convert to type-specific coefficients
    unsigned int i;
    for (i=0; i<h_len; i++)
        h[i] = (TC) hf[i];

    // create object and set parameters
    RRESAMP() q = RRESAMP(_create)(_P, _Q, _m, h);
    RRESAMP(_set_scale)(q, 2.0f*_bw*sqrtf((float)(q->Q)/(float)(q->P)));

    // free allocated memory and return object
    free(hf);
    free(h);
    return q;
}

// create rational-rate resampler object...
RRESAMP() RRESAMP(_create_prototype)(int          _type,
                                     unsigned int _P,
                                     unsigned int _Q,
                                     unsigned int _m,
                                     float        _beta)
{
    // design filter
    int          decim = _P < _Q;           // interpolator? or decimator
    unsigned int k     = decim ? _Q : _P;   // prototype samples per symbol
    unsigned int h_len = 2*k*_m + 1;        // filter length
    float * hf = (float*) malloc(h_len*sizeof(float));
    TC    * h  = (TC*)    malloc(h_len*sizeof(TC)   );
    liquid_firdes_prototype(_type, k, _m, _beta, 0, hf);

    // convert to type-specific coefficients
    unsigned int i;
    for (i=0; i<h_len; i++)
        h[i] = (TC) hf[i];

    // create object
    RRESAMP() q = RRESAMP(_create)(_P, _Q, _m, h);

    // adjust gain for decimator
    if (decim)
        RRESAMP(_set_scale)(q, (float)(q->P)/(float)(q->Q));

    // free allocated memory and return object
    free(hf);
    free(h);
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
    return RRESAMP(_create_kaiser)(_P, _Q, m, bw, As);
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
    printf("resampler [rate: %u/%u=%.6f, gcd=%u], m=%u\n",
            _q->P, _q->Q, (float)(_q->P) / (float)(_q->Q), _q->gcd, _q->m);
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

// Get greatest common divisor (g.c.d.) between original P and Q values
unsigned int RRESAMP(_get_gcd)(RRESAMP() _q)
{
    return _q->gcd;
}

// Get rate of resampler, r = P/Q
float RRESAMP(_get_rate)(RRESAMP() _q)
{
    return (float)(_q->P) / (float)(_q->Q);
}

// Get original interpolation factor of resampler, P
unsigned int RRESAMP(_get_P)(RRESAMP() _q)
{
    return _q->P * _q->gcd;
}

// Get interpolation factor of resampler, \(P\), after removing
// greatest common divisor
unsigned int RRESAMP(_get_interp)(RRESAMP() _q)
{
    return _q->P;
}

// Get original decimation factor of resampler, Q
unsigned int RRESAMP(_get_Q)(RRESAMP() _q)
{
    return _q->Q * _q->gcd;
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
    // run in blocks
    unsigned int i;
    for (i=0; i<_q->gcd; i++) {
        // compute P outputs for Q inputs
        RRESAMP(_execute_primitive)(_q, _x, _y);

        // update input pointers accordingly
        _x += _q->Q;
        _y += _q->P;
    }
}

// internal
void RRESAMP(_execute_primitive)(RRESAMP() _q,
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

#if 0
    // error checking for now
    assert(index == 0);
    assert(n == _q->P);
#endif
}

