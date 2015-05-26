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
// Automatic gain control
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "liquid.internal.h"

// default AGC loop bandwidth
#define AGC_DEFAULT_BW   (1e-2f)

// agc structure object
struct AGC(_s) {
    // gain variables
    T g;            // current gain value

    // gain control loop filter parameters
    float bandwidth;// bandwidth-time constant
    T alpha;        // feed-back gain

    // signal level estimate
    T y2_prime;     // filtered output signal energy estimate

    // AGC locked flag
    int is_locked;
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
    printf("agc [rssi: %12.4fdB]:\n", AGC(_get_rssi)(_q));
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
                            float _signal_level)
{
    // check to ensure signal level is reasonable
    if ( _signal_level <= 0 ) {
        fprintf(stderr,"error: agc_%s_set_signal_level(), bandwidth must be greater than zero\n", EXTENSION_FULL);
        exit(-1);
    }

    // set internal gain appropriately
    _q->g = 1.0f / _signal_level;

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

