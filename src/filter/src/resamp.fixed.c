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
// Arbitrary resampler (fixed-point phase verions)
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DEBUG_RESAMP_PRINT  0

// main object
struct RESAMP(_s) {
    // filter design parameters
    unsigned int    m;      // filter semi-length, h_len = 2*m + 1
    float           As;     // filter stop-band attenuation
    float           fc;     // filter cutoff frequency

    // internal state variables
    float           r;      // resampling rate
    uint32_t        step;   // step size (quantized resampling rate)
    uint32_t        phase;  // sampling phase
    unsigned int    npfb;   // 256
    FIRPFB()        pfb;    // filter bank
};

// create arbitrary resampler
//  _rate   :   resampling rate
//  _m      :   prototype filter semi-length
//  _fc     :   prototype filter cutoff frequency, fc in (0, 0.5)
//  _As     :   prototype filter stop-band attenuation [dB] (e.g. 60)
//  _npfb   :   number of filters in polyphase filterbank
RESAMP() RESAMP(_create)(float        _rate,
                         unsigned int _m,
                         float        _fc,
                         float        _As,
                         unsigned int _npfb)
{
    // validate input
    if (_rate <= 0) {
        fprintf(stderr,"error: resamp_%s_create(), resampling rate must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    } else if (_m == 0) {
        fprintf(stderr,"error: resamp_%s_create(), filter semi-length must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    } else if (_fc <= 0.0f || _fc >= 0.5f) {
        fprintf(stderr,"error: resamp_%s_create(), filter cutoff must be in (0,0.5)\n", EXTENSION_FULL);
        exit(1);
    } else if (_As <= 0.0f) {
        fprintf(stderr,"error: resamp_%s_create(), filter stop-band suppression must be greater than zero\n", EXTENSION_FULL);
        exit(1);
#if 0
    } else if (_npfb == 0) {
        fprintf(stderr,"error: resamp_%s_create(), number of filter banks must be greater than zero\n", EXTENSION_FULL);
        exit(1);
#endif
    }

    // allocate memory for resampler
    RESAMP() q = (RESAMP()) malloc(sizeof(struct RESAMP(_s)));

    // set rate using formal method (specifies output stride
    // value 'del')
    RESAMP(_set_rate)(q, _rate);

    // set properties
    q->m    = _m;       // prototype filter semi-length
    q->fc   = _fc;      // prototype filter cutoff frequency
    q->As   = _As;      // prototype filter stop-band attenuation
    q->npfb = 256;      // number of filters in bank (fixed 8-bit value)

    // design filter
    unsigned int n = 2*q->m*q->npfb+1;
    float hf[n];
    TC h[n];
    liquid_firdes_kaiser(n,q->fc/((float)(q->npfb)),q->As,0.0f,hf);

    // normalize filter coefficients by DC gain
    unsigned int i;
    float gain=0.0f;
    for (i=0; i<n; i++)
        gain += hf[i];
    gain = (q->npfb)/(gain);

    // copy to type-specific array, applying gain
    for (i=0; i<n; i++)
        h[i] = hf[i]*gain;
    q->pfb = FIRPFB(_create)(q->npfb,h,n-1);

    // reset object and return
    RESAMP(_reset)(q);
    return q;
}

// create arbitrary resampler object with a specified input
// resampling rate and default parameters
//  m (filter semi-length) = 7
//  fc (filter cutoff frequency) = 0.25
//  As (filter stop-band attenuation) = 60 dB
//  npfb (number of filters in the bank) = 256
RESAMP() RESAMP(_create_default)(float _rate)
{
    // validate input
    if (_rate <= 0) {
        fprintf(stderr,"error: resamp_%s_create_default(), resampling rate must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    }

    // det default parameters
    unsigned int m    = 7;
    float        fc   = 0.25f;
    float        As   = 60.0f;
    unsigned int npfb = 256;

    // create and return resamp object
    return RESAMP(_create)(_rate, m, fc, As, npfb);
}

// free arbitrary resampler object
void RESAMP(_destroy)(RESAMP() _q)
{
    // free polyphase filterbank
    FIRPFB(_destroy)(_q->pfb);

    // free main object memory
    free(_q);
}

// print resampler object
void RESAMP(_print)(RESAMP() _q)
{
    printf("resampler [rate: %f]\n", _q->r);
    FIRPFB(_print)(_q->pfb);
}

// reset resampler object
void RESAMP(_reset)(RESAMP() _q)
{
    // clear filterbank
    FIRPFB(_reset)(_q->pfb);

    // reset state
    _q->phase = 0;
}

// get resampler filter delay (semi-length m)
unsigned int RESAMP(_get_delay)(RESAMP() _q)
{
    return _q->m;
}

// set rate of arbitrary resampler
//  _q      : resampling object
//  _rate   : new sampling rate, _rate > 0
void RESAMP(_set_rate)(RESAMP() _q,
                       float    _rate)
{
    if (_rate <= 0) {
        fprintf(stderr,"error: resamp_%s_set_rate(), resampling rate must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    }

    // set internal rate
    _q->r = _rate;

    // set output stride
    _q->step = (uint32_t)round((1<<24)/_q->r);
}

// get rate of arbitrary resampler
float RESAMP(_get_rate)(RESAMP() _q)
{
    return _q->r;
}

// adjust resampling rate
void RESAMP(_adjust_rate)(RESAMP() _q,
                          float    _gamma)
{
    if (_gamma <= 0) {
        fprintf(stderr,"error: resamp_%s_adjust_rate(), resampling adjustment (%12.4e) must be greater than zero\n", EXTENSION_FULL, _gamma);
        exit(1);
    }

    // adjust internal rate
    RESAMP(_set_rate)(_q, _q->r * _gamma);
}


// set resampling timing phase
//  _q      : resampling object
//  _tau    : sample timing
void RESAMP(_set_timing_phase)(RESAMP() _q,
                               float    _tau)
{
    if (_tau < -1.0f || _tau > 1.0f) {
        fprintf(stderr,"error: resamp_%s_set_timing_phase(), timing phase must be in [-1,1], is %f\n.",
                EXTENSION_FULL, _tau);
        exit(1);
    }

    // TODO: set internal timing phase (quantized)
    //_q->tau = _tau;
}

// adjust resampling timing phase
//  _q      : resampling object
//  _delta  : sample timing adjustment
void RESAMP(_adjust_timing_phase)(RESAMP() _q,
                                  float    _delta)
{
    if (_delta < -1.0f || _delta > 1.0f) {
        fprintf(stderr,"error: resamp_%s_adjust_timing_phase(), timing phase adjustment must be in [-1,1], is %f\n.",
                EXTENSION_FULL, _delta);
        exit(1);
    }

    // TODO: adjust internal timing phase (quantized)
    //_q->tau += _delta;
}

// run arbitrary resampler
//  _q          :   resampling object
//  _x          :   single input sample
//  _y          :   output array
//  _num_written:   number of samples written to output
void RESAMP(_execute)(RESAMP()       _q,
                      TI             _x,
                      TO *           _y,
                      unsigned int * _num_written)
{
    // push input
    FIRPFB(_push)(_q->pfb, _x);

    // continue to produce output
    unsigned int n=0;
    while (_q->phase <= 0x00ffffff) {
        //unsigned int index = (_q->phase + 0x00008000) >> 16;
        unsigned int index = _q->phase >> 16; // round down
        FIRPFB(_execute)(_q->pfb, index, &_y[n++]);
        _q->phase += _q->step;
    }

    // decrement filter-bank index by output rate
    _q->phase -= (1<<24);

    // error checking for now
    *_num_written = n;
}

// execute arbitrary resampler on a block of samples
//  _q              :   resamp object
//  _x              :   input buffer [size: _nx x 1]
//  _nx             :   input buffer
//  _y              :   output sample array (pointer)
//  _ny             :   number of samples written to _y
void RESAMP(_execute_block)(RESAMP()       _q,
                            TI *           _x,
                            unsigned int   _nx,
                            TO *           _y,
                            unsigned int * _ny)
{
    // initialize number of output samples to zero
    unsigned int ny = 0;

    // number of samples written for each individual iteration
    unsigned int num_written;

    // iterate over each input sample
    unsigned int i;
    for (i=0; i<_nx; i++) {
        // run resampler on single input
        RESAMP(_execute)(_q, _x[i], &_y[ny], &num_written);

        // update output counter
        ny += num_written;
    }

    // set return value for number of output samples written
    *_ny = ny;
}

