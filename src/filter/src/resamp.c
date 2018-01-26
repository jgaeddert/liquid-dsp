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

// internal: update timing
void RESAMP(_update_timing_state)(RESAMP() _q);

struct RESAMP(_s) {
    // filter design parameters
    unsigned int m;     // filter semi-length, h_len = 2*m + 1
    float As;           // filter stop-band attenuation
    float fc;           // filter cutoff frequency

    // resampling properties/states
    float rate;         // resampling rate (ouput/input)
    float del;          // fractional delay step

    // floating-point phase
    float tau;          // accumulated timing phase, 0 <= tau < 1
    float bf;           // soft filterbank index, bf = tau*npfb = b + mu
    int b;              // base filterbank index, 0 <= b < npfb
    float mu;           // fractional filterbank interpolation value, 0 <= mu < 1
    TO y0;              // filterbank output at index b
    TO y1;              // filterbank output at index b+1

    // polyphase filterbank properties/object
    unsigned int npfb;  // number of filters in the bank
    FIRPFB() f;         // filterbank object (interpolator)

    enum {
        RESAMP_STATE_BOUNDARY, // boundary between input samples
        RESAMP_STATE_INTERP,   // regular interpolation
    } state;
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

    // set rate using formal method (specifies output stride
    // value 'del')
    RESAMP(_set_rate)(q, _rate);

    // set properties
    q->m    = _m;       // prototype filter semi-length
    q->fc   = _fc;      // prototype filter cutoff frequency
    q->As   = _As;      // prototype filter stop-band attenuation
    q->npfb = _npfb;    // number of filters in bank

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

// create arbitrary resampler object with a specified input
// resampling rate and default parameters
//  m (filter semi-length) = 7
//  fc (filter cutoff frequency) = 0.25
//  As (filter stop-band attenuation) = 60 dB
//  npfb (number of filters in the bank) = 64
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
    unsigned int npfb = 64;

    // create and return resamp object
    return RESAMP(_create)(_rate, m, fc, As, npfb);
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
    FIRPFB(_reset)(_q->f);

    // reset states
    _q->state = RESAMP_STATE_INTERP;// input/output sample state
    _q->tau   = 0.0f;           // accumulated timing phase
    _q->bf    = 0.0f;           // soft-valued filterbank index
    _q->b     = 0;              // base filterbank index
    _q->mu    = 0.0f;           // fractional filterbank interpolation value

    _q->y0    = 0;              // filterbank output at index b
    _q->y1    = 0;              // filterbank output at index b+1
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
    _q->rate = _rate;

    // set output stride
    _q->del = 1.0f / _q->rate;
}

// adjust resampling rate
void RESAMP(_adjust_rate)(RESAMP() _q,
                          float    _delta)
{
    if (_delta > 0.1f || _delta < -0.1f) {
        fprintf(stderr,"error: resamp_%s_adjust_rate(), resampling rate must be in [-0.1,0.1]\n", EXTENSION_FULL);
        exit(1);
    }

    // adjust internal rate
    _q->rate += _delta;

    // clip rate
    if (_q->rate >  0.5f) _q->rate =  0.5f;
    if (_q->rate < -0.5f) _q->rate = -0.5f;

    // set output stride
    _q->del = 1.0f / _q->rate;
}


// set resampling timing phase
//  _q      : resampling object
//  _tau    : sample timing
void RESAMP(_set_timing_phase)(RESAMP() _q,
                               float    _tau)
{
    if (_tau > 1.0f || _tau < -1.0f) {
        fprintf(stderr,"error: resamp_%s_set_timing_phase(), timing phase must be in [-1,1], is %f\n.",
                EXTENSION_FULL, _tau);
        exit(1);
    }

    // set internal timing phase
    _q->tau = _tau;
}

// adjust resampling timing phase
//  _q      : resampling object
//  _delta  : sample timing adjustment
void RESAMP(_adjust_timing_phase)(RESAMP() _q,
                                  float    _delta)
{
    if (_delta > 1.0f || _delta < -1.0f) {
        fprintf(stderr,"error: resamp_%s_adjust_timing_phase(), timing phase adjustment must be in [-1,1], is %f\n.",
                EXTENSION_FULL, _delta);
        exit(1);
    }

    // adjust internal timing phase
    _q->tau += _delta;
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
        case RESAMP_STATE_BOUNDARY:
            // compute filterbank output
            FIRPFB(_execute)(_q->f, 0, &_q->y1);

            // interpolate
            _y[n++] = (1.0f - _q->mu)*_q->y0 + _q->mu*_q->y1;
        
            // update timing state
            RESAMP(_update_timing_state)(_q);

            _q->state = RESAMP_STATE_INTERP;
            break;

        case RESAMP_STATE_INTERP:
            // compute output at base index
            FIRPFB(_execute)(_q->f, _q->b, &_q->y0);

            // check to see if base index is last filter in the bank, in
            // which case the resampler needs an additional input sample
            // to finish the linear interpolation process
            if (_q->b == _q->npfb-1) {
                // last filter: need additional input sample
                _q->state = RESAMP_STATE_BOUNDARY;
            
                // set index to indicate new sample is needed
                _q->b = _q->npfb;
            } else {
                // do not need additional input sample; compute
                // output at incremented base index
                FIRPFB(_execute)(_q->f, _q->b+1, &_q->y1);

                // perform linear interpolation between filterbank outputs
                _y[n++] = (1.0f - _q->mu)*_q->y0 + _q->mu*_q->y1;

                // update timing state
                RESAMP(_update_timing_state)(_q);
            }
            break;
        default:
            fprintf(stderr,"error: resamp_%s_execute(), invalid/unknown state\n", EXTENSION_FULL);
            exit(1);
        }
    }

    // decrement timing phase by one sample
    _q->tau -= 1.0f;
    _q->bf  -= (float)(_q->npfb);
    _q->b   -= _q->npfb;

    // specify number of samples written
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

// execute arbitrary resampler into specified output size
// will fill up to but not more than requested size _ny
// returns true if more samples can be fetched without more inputs
// false otherwise
//  _q              :   resamp object
//  _x              :   input buffer [size: _nx x 1]
//  _nx             :   input buffer size
//  _ux             :   number of input samples read
//  _y              :   output sample array
//  _ny             :   capacity of output sample array
//  _uy             :   number of output samples written
int RESAMP(_execute_output_block)(RESAMP() _q,
                                   const TI * _x,
                                   unsigned int _nx,
                                   unsigned int * _ux,
                                   TO * _y,
                                   unsigned int _ny,
                                   unsigned int * _uy)
{

    unsigned int num_read=0;
    unsigned int num_written=0;
    // loop through filling one output sample at a time
    while (num_written < _ny) {
        if (_q->b == 0 && _q->mu == 0) {
            // very beginning of filter -- we need an input to get started
            // this case seems strange, but we can't get here after things are moving
            // if later we land exactly on _q->b = npfb and _q->mu = 0 then
            //    we will get into the next, case, it will reset us to (0, 0),
            //    but then immediately after we write a new output that put us
            //    on some other b, mu
            if (num_read == _nx) {
                // ran out of input samples
                break;
            }
            // push input sample into filterbank
            FIRPFB(_push)(_q->f, _x[num_read]);
            num_read++;
        }
        int done = 0;
        while (_q->b >= _q->npfb) {
            // we have stepped through the filter so we need a new input
            if (num_read == _nx) {
                // ran out of input samples
                done = 1;
                break;
            }
            // decrement timing phase by one sample
            _q->tau -= 1.0f;
            _q->bf  -= (float)(_q->npfb);
            _q->b   -= _q->npfb;
            // push input sample into filterbank
            FIRPFB(_push)(_q->f, _x[num_read]);
            num_read++;
        }
        if (done) {
            break;
        }

        switch (_q->state) {
        case RESAMP_STATE_BOUNDARY:
            // compute filterbank output
            FIRPFB(_execute)(_q->f, 0, &_q->y1);

            // interpolate
            _y[num_written++] = (1.0f - _q->mu)*_q->y0 + _q->mu*_q->y1;

            // update timing state
            RESAMP(_update_timing_state)(_q);

            _q->state = RESAMP_STATE_INTERP;
            break;

        case RESAMP_STATE_INTERP:
            // compute output at base index
            FIRPFB(_execute)(_q->f, _q->b, &_q->y0);

            // check to see if base index is last filter in the bank, in
            // which case the resampler needs an additional input sample
            // to finish the linear interpolation process
            if (_q->b == _q->npfb-1) {
                // last filter: need additional input sample
                _q->state = RESAMP_STATE_BOUNDARY;

                // set index to indicate new sample is needed
                _q->b = _q->npfb;
            } else {
                // do not need additional input sample; compute
                // output at incremented base index
                FIRPFB(_execute)(_q->f, _q->b+1, &_q->y1);

                // perform linear interpolation between filterbank outputs
                _y[num_written++] = (1.0f - _q->mu)*_q->y0 + _q->mu*_q->y1;

                // update timing state
                RESAMP(_update_timing_state)(_q);
            }
            break;
        default:
            fprintf(stderr,"error: resamp_%s_execute(), invalid/unknown state\n", EXTENSION_FULL);
            exit(1);
        }
    }

    // specify number of samples used
    *_ux = num_read;
    *_uy = num_written;

    return _q->b < _q->npfb;
}


//
// internal methods
// 

// update timing state; increment output timing stride and
// quantize filterbank indices
void RESAMP(_update_timing_state)(RESAMP() _q)
{
    // update high-resolution timing phase
    _q->tau += _q->del;

    // convert to high-resolution filterbank index 
    _q->bf  = _q->tau * (float)(_q->npfb);

    // split into integer filterbank index and fractional interpolation
    _q->b   = (int)floorf(_q->bf);      // base index
    _q->mu  = _q->bf - (float)(_q->b);  // fractional index
}

