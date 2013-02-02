/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2013 Joseph Gaeddert
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
// Arbitrary resampler
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// defined:
//  TO          output data type
//  TC          coefficient data type
//  TI          input data type
//  RESAMP()    name-mangling macro
//  FIRPFB()    firpfb macro

// enable run-time debug printing of resampler
#define DEBUG_RESAMP_PRINT              0

// use fixed-point phase vs. floating-point phase
#define RESAMP_USE_FIXED_POINT_PHASE    1

struct RESAMP(_s) {
    // filter design parameters
    unsigned int m;     // filter semi-length, h_len = 2*m + 1
    float As;           // filter stop-band attenuation
    float fc;           // filter cutoff frequency

    // resampling properties/states
    float rate;         // resampling rate (ouput/input)
    int b;              // filterbank index
    float del;          // fractional delay step

#if RESAMP_USE_FIXED_POINT_PHASE
    // fixed-point phase
    unsigned int theta;             // sampling phase
    unsigned int d_theta;           // phase differential

    unsigned int num_bits_phase;    // number of bits in phase
    unsigned int max_phase;         // maximum phase value
    unsigned int num_bits_npfb;     // number of bits in npfb
    unsigned int num_shift_bits;    // number of bits to shift to compute b
#else
    // floating-point phase
    float tau;      // accumulated timing phase (0 <= tau <= 1)
    float bf;       // soft filterbank index
#endif

    // polyphase filterbank properties/object
    unsigned int npfb;  // number of filters in the bank
    FIRPFB() f;         // actual filterbank object
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
    } else if (_npfb == 0) {
        fprintf(stderr,"error: resamp_%s_create(), number of filter banks must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    } else if (_fc <= 0.0f || _fc >= 0.5f) {
        fprintf(stderr,"error: resamp_%s_create(), filter cutoff must be in (0,0.5)\n", EXTENSION_FULL);
        exit(1);
    } else if (_As <= 0.0f) {
        fprintf(stderr,"error: resamp_%s_create(), filter stop-band suppression must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    }

    // allocate memory for resampler
    RESAMP() q = (RESAMP()) malloc(sizeof(struct RESAMP(_s)));

    // set properties
    q->rate  = _rate;       // resampling rate
    q->m     = _m;          // prototype filter semi-length
    q->fc    = _fc;         // prototype filter cutoff frequency
    q->As    = _As;         // prototype filter stop-band attenuation

    // set derived values
    q->b   = 0;             // initial filterbank index
    q->del = 1.0f / q->rate;// timing phase step size (reciprocal of rate)
    printf("rate = %12.8f\n", q->rate);
    printf("del  = %12.8f\n", q->del);

#if RESAMP_USE_FIXED_POINT_PHASE
    // using fixed-point phase, increase number of filters in bank
    q->num_bits_npfb = liquid_nextpow2(_npfb);
    q->npfb = 1<<q->num_bits_npfb;
    q->num_bits_phase = 20;
    q->max_phase = 1 << q->num_bits_phase;
    q->num_shift_bits = q->num_bits_phase - q->num_bits_npfb;
    q->theta = 0;
    q->d_theta = (unsigned int)( q->max_phase * q->del );
#else
    // set other derived values
    q->npfb = _npfb;        // number of filters in bank
    q->tau  = 0.0f;         // accumulated timing phase
    q->bf   = 0.0f;         // floating-point filterbank index
#endif

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
    q->f = FIRPFB(_create)(q->npfb,h,n-1);

    // return object
    return q;
}

// free arbitrary resampler object
void RESAMP(_destroy)(RESAMP() _q)
{
    // free polyphase filterbank
    FIRPFB(_destroy)(_q->f);

    // free main object memory
    free(_q);
}

// print resampler object
void RESAMP(_print)(RESAMP() _q)
{
    printf("resampler [rate: %f]\n", _q->rate);
    FIRPFB(_print)(_q->f);
}

// reset resampler object
void RESAMP(_reset)(RESAMP() _q)
{
    // clear filterbank
    FIRPFB(_clear)(_q->f);

    // reset states
    _q->b   = 0;
#if RESAMP_USE_FIXED_POINT_PHASE
    _q->theta = 0;
#else
    _q->tau = 0.0f;
    _q->bf  = 0.0f;
#endif
}

// set rate
void RESAMP(_setrate)(RESAMP() _q, float _rate)
{
    // TODO : validate rate, validate this method
    _q->rate = _rate;
    _q->del = 1.0f / _q->rate;

#if RESAMP_USE_FIXED_POINT_PHASE
    _q->d_theta = (unsigned int)( _q->max_phase * _q->del );
#endif
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
#if RESAMP_USE_FIXED_POINT_PHASE
{
    // push input sample into filterbank
    FIRPFB(_push)(_q->f, _x);
    unsigned int n=0;
    
    //while (_q->tau < 1.0f) {
    while (_q->theta < _q->max_phase) {

#if DEBUG_RESAMP_PRINT
        printf("  [%2u] : theta = %8u, b : %6u\n", n, _q->theta, _q->b);
#endif
        FIRPFB(_execute)(_q->f, _q->b, &_y[n]);

        _q->theta += _q->d_theta;
        _q->b = _q->theta >> _q->num_shift_bits;
        n++;
    }

    _q->theta -= _q->max_phase;
    _q->b = _q->theta >> _q->num_shift_bits;

    *_num_written = n;
}
#else
{
    // push input sample into filterbank
    FIRPFB(_push)(_q->f, _x);
    unsigned int n=0;
    
    //while (_q->tau < 1.0f) {
    while (_q->b < _q->npfb) {

#if DEBUG_RESAMP_PRINT
        printf("  [%2u] : tau : %12.8f, b : %4u (%12.8f)\n", n+1, _q->tau, _q->b, _q->bf);
#endif
        FIRPFB(_execute)(_q->f, _q->b, &_y[n]);

        _q->tau += _q->del;
        _q->bf = _q->tau * (float)(_q->npfb);
        _q->b  = (int)roundf(_q->bf);
        n++;
    }

    _q->tau -= 1.0f;
    _q->bf  -= (float)(_q->npfb);
    _q->b   -= _q->npfb;

    *_num_written = n;
}
#endif

