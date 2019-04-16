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
// Automatic gain control
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "liquid.internal.h"

// default AGC loop bandwidth
#define AGC_DEFAULT_BW   (1e-2f)

// internal method definition
void AGC(_squelch_update_mode)(AGC() _q);

// agc structure object
struct AGC(_s) {
    // gain variables
    T g;            // current gain value
    T scale;        // output scale value

    // gain control loop filter parameters
    float bandwidth;// bandwidth-time constant
    T alpha;        // feed-back gain

    // signal level estimate
    T y2_prime;     // filtered output signal energy estimate

    // AGC locked flag
    int is_locked;

    // squelch mode
    agc_squelch_mode squelch_mode;

    // squelch threshold
    T squelch_threshold;

    // squelch timeout
    unsigned int squelch_timeout;
    unsigned int squelch_timer;
};

// create agc object
AGC() AGC(_create)(void)
{
    // create object and initialize to default parameters
    AGC() _q = (AGC()) malloc(sizeof(struct AGC(_s)));

    // initialize bandwidth
    AGC(_set_bandwidth)(_q, AGC_DEFAULT_BW);

    // reset object
    AGC(_reset)(_q);

    // squelch
    AGC(_squelch_disable)(_q);
    AGC(_squelch_set_threshold)(_q, 0.0f);
    AGC(_squelch_set_timeout  )(_q, 100);

    // set default output gain
    _q->scale = 1;

    // return object
    return _q;
}

// destroy agc object, freeing all internally-allocated memory
void AGC(_destroy)(AGC() _q)
{
    // free main object memory
    free(_q);
}

// print agc object internals
void AGC(_print)(AGC() _q)
{
    printf("agc [rssi: %12.4f dB, output gain: %.3f dB, bw: %12.4e, locked: %s, squelch: %s]:\n",
            AGC(_get_rssi)(_q),
            _q->scale > 0 ? 10.*log10f(_q->scale) : -100.0f,
            _q->bandwidth,
            _q->is_locked ? "yes" : "no",
            _q->squelch_mode == LIQUID_AGC_SQUELCH_DISABLED ? "disabled" : "enabled");
}

// reset agc object's internal state
void AGC(_reset)(AGC() _q)
{
    // reset gain estimate
    _q->g = 1.0f;

    // reset signal level estimate
    _q->y2_prime = 1.0f;

    // unlock gain control
    AGC(_unlock)(_q);

    // reset squelch state
    _q->squelch_mode = (_q->squelch_mode == LIQUID_AGC_SQUELCH_DISABLED) ?
        LIQUID_AGC_SQUELCH_DISABLED : LIQUID_AGC_SQUELCH_ENABLED;
}

// execute automatic gain control loop
//  _q      :   agc object
//  _x      :   input sample
//  _y      :   output sample
void AGC(_execute)(AGC() _q,
                   TC    _x,
                   TC *  _y)
{
    // apply gain to input sample
    *_y = _x * _q->g;

    // compute output signal energy
    T y2 = crealf( (*_y)*conjf(*_y) );

    // smooth energy estimate using single-pole low-pass filter
    _q->y2_prime = (1.0-_q->alpha)*_q->y2_prime + _q->alpha*y2;

    // return if locked
    if (_q->is_locked)
        return;

    // update gain according to output energy
    if (_q->y2_prime > 1e-6f)
        _q->g *= expf( -0.5f*_q->alpha*logf(_q->y2_prime) );

    // clamp to 120 dB gain
    if (_q->g > 1e6f)
        _q->g = 1e6f;

    // udpate squelch mode appropriately
    AGC(_squelch_update_mode)(_q);

    // apply output scale
    *_y *= _q->scale;
}

// execute automatic gain control on block of samples
//  _q      : automatic gain control object
//  _x      : input data array, [size: _n x 1]
//  _n      : number of input, output samples
//  _y      : output data array, [size: _n x 1]
void AGC(_execute_block)(AGC()        _q,
                         TC *         _x,
                         unsigned int _n,
                         TC *         _y)
{
    unsigned int i;
    for (i=0; i<_n; i++)
        AGC(_execute)(_q, _x[i], &_y[i]);
}

// lock agc
void AGC(_lock)(AGC() _q)
{
    _q->is_locked = 1;
}

// unlock agc
void AGC(_unlock)(AGC() _q)
{
    _q->is_locked = 0;
}

// get agc loop bandwidth
float AGC(_get_bandwidth)(AGC() _q)
{
    return _q->bandwidth;
}

// set agc loop bandwidth
//  _q      :   agc object
//  _BT     :   bandwidth
void AGC(_set_bandwidth)(AGC() _q,
                         float _bt)
{
    // check to ensure bandwidth is reasonable
    if ( _bt < 0 ) {
        fprintf(stderr,"error: agc_%s_set_bandwidth(), bandwidth must be positive\n", EXTENSION_FULL);
        exit(-1);
    } else if ( _bt > 1.0f ) {
        fprintf(stderr,"error: agc_%s_set_bandwidth(), bandwidth must less than 1.0\n", EXTENSION_FULL);
        exit(-1);
    }

    // set internal bandwidth
    _q->bandwidth = _bt;

    // compute filter coefficient based on bandwidth
    _q->alpha = _q->bandwidth;
}

// get estimated signal level (linear)
float AGC(_get_signal_level)(AGC() _q)
{
    return 1.0f / _q->g;
}

// set estimated signal level (linear)
void AGC(_set_signal_level)(AGC() _q,
                            float _x2)
{
    // check to ensure signal level is reasonable
    if ( _x2 <= 0 ) {
        fprintf(stderr,"error: agc_%s_set_signal_level(), bandwidth must be greater than zero\n", EXTENSION_FULL);
        exit(-1);
    }

    // set internal gain appropriately
    _q->g = 1.0f / _x2;

    // reset internal output signal level
    _q->y2_prime = 1.0f;
}

// get estimated signal level (dB)
float AGC(_get_rssi)(AGC() _q)
{
    return -20*log10(_q->g);
}

// set estimated signal level (dB)
void AGC(_set_rssi)(AGC() _q,
                    float _rssi)
{
    // set internal gain appropriately
    _q->g = powf(10.0f, -_rssi/20.0f);

    // ensure resulting gain is not arbitrarily low
    if (_q->g < 1e-16f)
        _q->g = 1e-16f;

    // reset internal output signal level
    _q->y2_prime = 1.0f;
}

// get internal gain
float AGC(_get_gain)(AGC() _q)
{
    return _q->g;
}

// set internal gain
void AGC(_set_gain)(AGC() _q,
                    float _gain)
{
    // check to ensure gain is reasonable
    if ( _gain <= 0 ) {
        fprintf(stderr,"error: agc_%s_set_gain(), gain must be greater than zero\n", EXTENSION_FULL);
        exit(-1);
    }

    // set internal gain appropriately
    _q->g = _gain;
}

// get scale
float AGC(_get_scale)(AGC() _q)
{
    return _q->scale;
}

// set scale
void AGC(_set_scale)(AGC() _q,
                     float _scale)
{
    // check to ensure gain is reasonable
    if ( _scale <= 0 ) {
        fprintf(stderr,"error: agc_%s_set_scale(), scale must be greater than zero\n", EXTENSION_FULL);
        exit(-1);
    }

    // set internal gain appropriately
    _q->scale = _scale;
}

// initialize internal gain on input array
//  _q      : automatic gain control object
//  _x      : input data array, [size: _n x 1]
//  _n      : number of input, output samples
void AGC(_init)(AGC()        _q,
                TC *         _x,
                unsigned int _n)
{
    // ensure number of samples is greater than zero
    if ( _n == 0 ) {
        fprintf(stderr,"error: agc_%s_init(), number of samples must be greater than zero\n", EXTENSION_FULL);
        exit(-1);
    }

    // compute sum squares on input
    // TODO: use vector methods for this
    unsigned int i;
    T x2 = 0;
    for (i=0; i<_n; i++)
        x2 += crealf( _x[i]*conjf(_x[i]) );

    // compute RMS level and ensure result is positive
    x2 = sqrtf( x2 / (float) _n ) + 1e-16f;

    // set internal gain based on estimated signal level
    AGC(_set_signal_level)(_q, x2);
}

// enable squelch mode
void AGC(_squelch_enable)(AGC() _q)
{
    _q->squelch_mode = LIQUID_AGC_SQUELCH_ENABLED;
}

// disable squelch mode
void AGC(_squelch_disable)(AGC() _q)
{
    _q->squelch_mode = LIQUID_AGC_SQUELCH_DISABLED;
}

// is squelch enabled?
int  AGC(_squelch_is_enabled)(AGC() _q)
{
    return _q->squelch_mode == LIQUID_AGC_SQUELCH_DISABLED ? 0 : 1;
}

// set squelch threshold
//  _q          :   automatic gain control object
//  _thresh_dB  :   threshold for enabling squelch [dB]
void AGC(_squelch_set_threshold)(AGC() _q,
                                 T     _threshold)
{
    _q->squelch_threshold = _threshold;
}

// get squelch threshold [dB]
T AGC(_squelch_get_threshold)(AGC() _q)
{
    return _q->squelch_threshold;
}

// set squelch timeout
//  _q       : automatic gain control object
//  _timeout : timeout before enabling squelch [samples]
void AGC(_squelch_set_timeout)(AGC()        _q,
                               unsigned int _timeout)
{
    _q->squelch_timeout = _timeout;
}

// get squelch timeout [samples]
unsigned int AGC(_squelch_get_timeout)(AGC() _q)
{
    return _q->squelch_timeout;
}

// get squelch mode
int AGC(_squelch_get_status)(AGC() _q)
{
    return _q->squelch_mode;
}

//
// internal methods
//

// update squelch mode appropriately
void AGC(_squelch_update_mode)(AGC() _q)
{
    //
    int threshold_exceeded = (AGC(_get_rssi)(_q) > _q->squelch_threshold);

    // update state
    switch (_q->squelch_mode) {
    case LIQUID_AGC_SQUELCH_ENABLED:
        _q->squelch_mode = threshold_exceeded ? LIQUID_AGC_SQUELCH_RISE : LIQUID_AGC_SQUELCH_ENABLED;
        break;
    case LIQUID_AGC_SQUELCH_RISE:
        _q->squelch_mode = threshold_exceeded ? LIQUID_AGC_SQUELCH_SIGNALHI : LIQUID_AGC_SQUELCH_FALL;
        break;
    case LIQUID_AGC_SQUELCH_SIGNALHI:
        _q->squelch_mode = threshold_exceeded ? LIQUID_AGC_SQUELCH_SIGNALHI : LIQUID_AGC_SQUELCH_FALL;
        break;
    case LIQUID_AGC_SQUELCH_FALL:
        _q->squelch_mode = threshold_exceeded ? LIQUID_AGC_SQUELCH_SIGNALHI : LIQUID_AGC_SQUELCH_SIGNALLO;
        _q->squelch_timer = _q->squelch_timeout;
        break;
    case LIQUID_AGC_SQUELCH_SIGNALLO:
        _q->squelch_timer--;
        if (_q->squelch_timer == 0)
            _q->squelch_mode = LIQUID_AGC_SQUELCH_TIMEOUT;
        else if (threshold_exceeded)
            _q->squelch_mode = LIQUID_AGC_SQUELCH_SIGNALHI;
        break;
    case LIQUID_AGC_SQUELCH_TIMEOUT:
        _q->squelch_mode = LIQUID_AGC_SQUELCH_ENABLED;
        break;
    case LIQUID_AGC_SQUELCH_DISABLED:
        break;
    case LIQUID_AGC_SQUELCH_UNKNOWN:
    default:
        fprintf(stderr,"warning: agc_%s_execute(), invalid squelch mode: %d\n",
                EXTENSION_FULL, _q->squelch_mode);
    }
}

