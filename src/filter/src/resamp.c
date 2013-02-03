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

struct RESAMP(_s) {
    // filter design parameters
    unsigned int m;     // filter semi-length, h_len = 2*m + 1
    float As;           // filter stop-band attenuation
    float fc;           // filter cutoff frequency

    // resampling properties/states
    float rate;         // resampling rate (ouput/input)
    int b;              // filterbank index
    float del;          // fractional delay step

    // floating-point phase
    float tau;      // accumulated timing phase (0 <= tau <= 1)
    float bf;       // soft filterbank index

    // polyphase filterbank properties/object
    unsigned int npfb;  // number of filters in the bank
    FIRPFB() f;         // actual filterbank object

    int state;
    unsigned int b0;
    unsigned int b1;
    TO y0;
    TO y1;
    float mu;
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
#if DEBUG_RESAMP_PRINT
    printf("rate = %12.8f\n", q->rate);
    printf("del  = %12.8f\n", q->del);
#endif

    // set other derived values
    q->npfb = _npfb;        // number of filters in bank
    q->tau  = 0.0f;         // accumulated timing phase
    q->bf   = 0.0f;         // floating-point filterbank index

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

    // reset object and return
    RESAMP(_reset)(q);
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
    _q->tau = 0.0f;
    _q->bf  = 0.0f;
    _q->state = 2;
    _q->y0    = 0;
    _q->y1    = 0;
    _q->b0    = 0;
    _q->b1    = 1;
    _q->mu    = 0.0f;
}

// set rate
void RESAMP(_setrate)(RESAMP() _q, float _rate)
{
    // TODO : validate rate, validate this method
    _q->rate = _rate;
    _q->del = 1.0f / _q->rate;
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
    // push input sample into filterbank
    FIRPFB(_push)(_q->f, _x);
    unsigned int n=0;
    
    while (_q->b < _q->npfb) {

#if DEBUG_RESAMP_PRINT
        printf("  [%2u] : s=%1u, tau=%12.8f, b : %12.8f (%4d + %8.6f)\n", n+1, _q->state, _q->tau, _q->bf, _q->b, _q->mu);
#endif
        switch (_q->state) {
        case 1:
            //printf("  state 1\n");
            // compute filterbank output
            FIRPFB(_execute)(_q->f, 0, &_q->y1);

            // interpolate
            _y[n] = (1.0f - _q->mu)*_q->y0 + _q->mu*_q->y1;
        
            // update timing
            _q->tau += _q->del;
            _q->bf  = _q->tau * (float)(_q->npfb);
            _q->b   = (int)floorf(_q->bf);
            _q->mu  = _q->bf - (float)(_q->b);
            n++;

            // update state
            _q->b0 = _q->b;
            _q->b1 = _q->b + 1;
            _q->state = (_q->b1 == _q->npfb) ? 3 : 2;
            
            break;

        case 2:
            //printf("  state 2: b0 = %3u, b1=%3u, mu=%8.6f\n", _q->b0, _q->b1, _q->mu);
            // compute both outputs
            FIRPFB(_execute)(_q->f, _q->b0, &_q->y0);
            FIRPFB(_execute)(_q->f, _q->b1, &_q->y1);

            // interpolate...
            _y[n] = (1.0f - _q->mu)*_q->y0 + _q->mu*_q->y1;

            // update timing
            _q->tau += _q->del;
            _q->bf  = _q->tau * (float)(_q->npfb);
            _q->b   = (int)floorf(_q->bf);
            _q->mu  = _q->bf - (float)(_q->b);
            n++;

            // update state
            _q->b0 = _q->b;
            _q->b1 = _q->b + 1;
            _q->state = (_q->b1 == _q->npfb) ? 3 : 2;
            
            break;

        case 3:
            //printf("  state 3\n");

            // compute filterbank output
            FIRPFB(_execute)(_q->f, _q->b0, &_q->y0);

            // set filterbank to maximum to require new input sample
            _q->b  = _q->npfb;
            _q->b0 = _q->npfb;
            _q->b1 = 0;

            // set state
            _q->state = 1;

            break;
        default:
            fprintf(stderr,"error: resamp_%s_execute(), invalid/unknown state\n", EXTENSION_FULL);
            exit(1);
        }
    }

    _q->tau -= 1.0f;
    _q->bf  -= (float)(_q->npfb);
    _q->b   -= _q->npfb;
    
    _q->b0  -= _q->npfb;
    _q->b1  -= _q->npfb;

    *_num_written = n;
}

