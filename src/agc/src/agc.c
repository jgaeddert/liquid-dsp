/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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
// Automatic gain control
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "liquid.internal.h"

// squash output signal if squelch is activate
#define AGC_SQUELCH_GAIN 0

// forward declaration of internal methods
void AGC(_estimate_input_energy)(AGC() _q, TC _x);
void AGC(_limit_gain)(AGC() _q);
void AGC(_update_auto_squelch)(AGC() _q, T _rssi);
void AGC(_execute_squelch)(AGC() _q);


// agc structure object
struct AGC(_s) {
    // gain variables
    T g_hat;        // instantaneous gain estimate
    T g;            // current gain value
    T g_min;        // minimum gain value
    T g_max;        // maximum gain value
#if AGC_SQUELCH_GAIN
    T g_squelch;    // squelch gain
#endif

    // gain control loop filter parameters
    T BT;           // bandwidth-time constant
    T alpha;        // feed-back gain
    T beta;         // feed-forward gain

    // signal energy estimate iir filter state variables
    T e_hat;        // instantaneous estimated signal energy
    T gamma_hat;    // filtered (average) signal level estimate

    // is agc locked?
    int is_locked;

    // 'true' agc method
    T * buffer;                     // buffered |input values|^2
    T buffer_sum;                   // accumulated sum of buffer
    unsigned int buffer_len;        // size of input buffer
    unsigned int buffer_index;      // write index of input buffer
    T sqrt_buffer_len;              // sqrt(buffer_len)

    // squelch
    int squelch_activated;          // squelch activated/deactivated?
    int squelch_auto;               // automatic squelch?
    unsigned int squelch_timeout;   // number of samples before timing out
    unsigned int squelch_timer;     // sub-threshold counter
    T squelch_threshold_auto;       // squelch threshold (auto)
    T squelch_threshold;            // squelch threshold
    T squelch_headroom;             // nominally 4 dB
    int squelch_status;             // status
};

// create agc object
AGC() AGC(_create)(void)
{
    // create object and initialize to default parameters
    AGC() _q = (AGC()) malloc(sizeof(struct AGC(_s)));

    // initialize loop filter state variables
    _q->gamma_hat = 1.0f;

    // set default gain variables
#if defined LIQUID_FIXED
    _q->g_min   = Q(_max) >> 1;
    _q->g_max   = Q(_min) << 1;
#else
    _q->g_min   = 1e-6f;
    _q->g_max   = 1e+6f;
#endif

    // initialize internals
    AGC(_set_bandwidth)(_q, 0.0);
    _q->is_locked = 0;

    // create input buffer, initialize with zeros
    _q->buffer_len = 16;
    _q->sqrt_buffer_len = sqrtf(_q->buffer_len);
    _q->buffer = (T*) malloc((_q->buffer_len)*sizeof(T));

    // squelch
#if defined LIQUID_FIXED
    _q->squelch_headroom = Q(_float_to_fixed)(0.39811f);    // roughly 4dB
#else
    _q->squelch_headroom = 0.39811f;    // roughly 4dB
#endif
    AGC(_squelch_disable_auto)(_q);
    AGC(_squelch_set_threshold)(_q, -30.0f);
    AGC(_squelch_set_timeout)(_q, 32);
    AGC(_squelch_deactivate)(_q);

    // reset object
    AGC(_reset)(_q);

    // return object
    return _q;
}

// destroy agc object, freeing all internally-allocated memory
void AGC(_destroy)(AGC() _q)
{
    // free internal buffer
    free(_q->buffer);

    // free main object memory
    free(_q);
}

// print agc object internals
void AGC(_print)(AGC() _q)
{
#if defined LIQUID_FIXED
    printf("agc [rssi: %12.4f dB]:\n", Q(_fixed_to_float)( AGC(_get_rssi)(_q)) );
#else
    printf("agc [rssi: %12.4f dB]:\n", AGC(_get_rssi)(_q));
#endif
}

// reset agc object
void AGC(_reset)(AGC() _q)
{
    _q->gamma_hat   = 1.0f;
    _q->g_hat       = 1.0f;
    _q->g           = 1.0f;

    _q->buffer_index = 0;
    _q->buffer_sum = (T)(_q->buffer_len);
    unsigned int i;
    for (i=0; i<_q->buffer_len; i++)
        _q->buffer[i] = 1.0f;

#if AGC_SQUELCH_GAIN
    // set 'squelch' gain
    _q->g_squelch   = 1.0f;
#endif

    AGC(_unlock)(_q);
}

// set agc gain limits
//  _q      :   agc object
//  _g_min  :   minimum allowable gain
//  _g_max  :   maximum allowable gain
void AGC(_set_gain_limits)(AGC() _q,
                           T _g_min,
                           T _g_max)
{
    // validate input
    if (_g_min > _g_max) {
        fprintf(stderr,"error: agc_%s_set_gain_limits(), _g_min < _g_max\n", EXTENSION_FULL);
        exit(-1);
    }

    _q->g_min = _g_min;
    _q->g_max = _g_max;
}

// set agc loop bandwidth
//  _q      :   agc object
//  _BT     :   bandwidth
void AGC(_set_bandwidth)(AGC() _q,
                         T     _BT)
{
    // FIXME: implement this method for fixed-point math

    // check to ensure _BT is reasonable
    if ( _BT < 0 ) {
        fprintf(stderr,"error: agc_%s_set_bandwidth(), bandwidth must be positive\n", EXTENSION_FULL);
        exit(-1);
    } else if ( _BT > 1.0f ) {
        fprintf(stderr,"error: agc_%s_set_bandwidth(), bandwidth must less than 1.0\n", EXTENSION_FULL);
        exit(-1);
    }

    // set internal bandwidth
    _q->BT = _BT;

    // ensure normalized bandwidth is less than one
    T bt = _q->BT;
#if defined LIQUID_FIXED
    if (bt >= Q(_one))
        bt = Q(_float_to_fixed)(0.95f);
#else
    if (bt >= 1.0f) bt = 0.99f;
#endif

    // compute coefficients
    _q->alpha = sqrtf(bt);
    _q->beta = 1 - _q->alpha;
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

// push input sample, update internal tracking loop
//  _q      :   agc object
//  _x      :   input sample
void AGC(_push)(AGC() _q,
                TC    _x)
{
    // if agc is locked, just return (do nothing)
    if (_q->is_locked)
        return;

    // compute input energy estimate
    AGC(_estimate_input_energy)(_q, _x);

    // compute instantaneous gain
    _q->g_hat = _q->sqrt_buffer_len / (_q->gamma_hat + 1e-12f);

    // update gain according to recursive filter
    _q->g = (_q->beta)*(_q->g) + (_q->alpha)*_q->g_hat;

    // limit gain
    AGC(_limit_gain)(_q);

    // update squelch control, if activated
    if (_q->squelch_activated)
        AGC(_execute_squelch)(_q);
}

// apply gain to input sample
//  _q      :   agc object
//  _x      :   input/output sample
void AGC(_apply_gain)(AGC() _q,
                      TC *  _y)
{
    // apply internal gain to input
#if defined LIQUID_FIXED && TC_COMPLEX==1
    *_y = CQ(_mul_scalar)(*_y,_q->g);
#elif defined LIQUID_FIXED && TC_COMPLEX==0
    *_y = Q(_mul)(*_y,_q->g);
#else
    *_y *= _q->g;
#endif

#if AGC_SQUELCH_GAIN
    // apply squelch gain
    *_y *= _q->g_squelch;
#endif
}

// execute automatic gain control loop
//  _q      :   agc object
//  _x      :   input sample
//  _y      :   output sample
void AGC(_execute)(AGC() _q,
                   TC    _x,
                   TC *  _y)
{
    // push input sample, update internal tracking loop
    AGC(_push)(_q, _x);

    // apply gain to input
    *_y = _x;
    AGC(_apply_gain)(_q, _y);
}

// get estimated signal level (linear)
T AGC(_get_signal_level)(AGC() _q)
{
#if defined LIQUID_FIXED
    return Q(_inv)(_q->g, 8);
#else
    return (1.0 / _q->g);
#endif
}

// get estimated signal level (dB)
T AGC(_get_rssi)(AGC() _q)
{
#if defined LIQUID_FIXED
    return -20*Q(_log10)(_q->g);
#else
    return -20.0*log10(_q->g);
#endif
}

// get internal gain
T AGC(_get_gain)(AGC() _q)
{
    return _q->g;
}

// activate squelch
void AGC(_squelch_activate)(AGC() _q)
{
    _q->squelch_activated = 1;
#if AGC_SQUELCH_GAIN
#  if defined LIQUID_FIXED
    _q->g_squelch = Q(_one);
#  else
    _q->g_squelch = 1.0f;
#  endif
#endif
}

// deactivate squelch
void AGC(_squelch_deactivate)(AGC() _q)
{
    _q->squelch_activated = 0;
    _q->squelch_status = LIQUID_AGC_SQUELCH_SIGNALHI;
    _q->squelch_timer = _q->squelch_timeout;
}

// enable automatic squelch
void AGC(_squelch_enable_auto)(AGC() _q)
{
    _q->squelch_auto = 1;
}

// disenable automatic squelch
void AGC(_squelch_disable_auto)(AGC() _q)
{
    _q->squelch_auto = 0;
}

// set squelch threshold
//  _q          :   agc object
//  _threshold  :   squelch threshold level [dB]
void AGC(_squelch_set_threshold)(AGC() _q,
                                 T     _threshold)
{
#if defined LIQUID_FIXED
    // FIXME: implement this method for fixed-point math
#else
    _q->squelch_threshold      = powf(10.0f,_threshold / 20.0f);
    _q->squelch_threshold_auto = _q->squelch_threshold;
#endif
}

// get squelch threshold [dB]
T AGC(_squelch_get_threshold)(AGC() _q)
{
#if defined LIQUID_FIXED
    // FIXME: check this method for fixed-point math
    return 20*Q(_log10)(_q->squelch_threshold);
#else
    return 20.0f*log10f(_q->squelch_threshold);
#endif
}

// set squelch timeout (time before squelch is deactivated)
//  _q      :   agc object
//  _n      :   squelch timeout
void AGC(_squelch_set_timeout)(AGC() _q,
                               unsigned int _n)
{
    _q->squelch_timeout = _n;
}

// return squelch status code
int AGC(_squelch_get_status)(AGC() _q)
{
    return _q->squelch_status;
}


// 
// internal methods
//

// estimate signal input energy
//  _q      :   agc object
//  _x      :   input sample
void AGC(_estimate_input_energy)(AGC() _q,
                                 TC    _x)
{
    // compute instantaneous signal energy
#if defined LIQUID_FIXED && TC_COMPLEX==1
    _q->e_hat = Q(_mul)(_x.real, _x.real) + Q(_mul)(_x.imag, _x.imag);
#elif defined LIQUID_FIXED && TC_COMPLEX==0
    _q->e_hat = Q(_mul)(_x,_x);
#elif TC_COMPLEX==1
    _q->e_hat = crealf(_x)*crealf(_x) + cimagf(_x)*cimagf(_x);
#else
    _q->e_hat = _x*_x;
#endif

    // increment sum by |_x|^2
    _q->buffer_sum += _q->e_hat;

    // decrement sum by buffer value
    _q->buffer_sum -= _q->buffer[ _q->buffer_index ];

    // push sample into buffer
    _q->buffer[_q->buffer_index] = _q->e_hat;

    // increment index
    _q->buffer_index = (_q->buffer_index + 1) % _q->buffer_len;

    // ensure buffer_sum is non-negative
    if (_q->buffer_sum < 0) _q->buffer_sum = 0;

    // filter energy estimate
    _q->gamma_hat = sqrtf(_q->buffer_sum);
}

// limit gain
void AGC(_limit_gain)(AGC() _q)
{
    if ( _q->g > _q->g_max )
        _q->g = _q->g_max;
    else if ( _q->g < _q->g_min )
        _q->g = _q->g_min;
}

// update automatic squelch threshold
//  _q      :   agc object
//  _rssi   :   estimated received signal strength (linear)
void AGC(_update_auto_squelch)(AGC() _q,
                               T     _rssi)
{
#if defined LIQUID_FIXED
    // FIXME: test this method for fixed-point math
    if (_rssi < Q(_mul)(_q->squelch_threshold,_q->squelch_headroom) )
        _q->squelch_threshold = Q(_mul)(_q->squelch_threshold, Q(_float_to_fixed)(0.95f));
    else
        _q->squelch_threshold = Q(_mul)(_q->squelch_threshold, Q(_float_to_fixed)(1.01f));
#else
    // if rssi dips too low (roughly 4dB below threshold),
    // decrease threshold slightly
    if (_rssi < _q->squelch_threshold * _q->squelch_headroom) {
        _q->squelch_threshold *= 0.95f;
#if 0
        printf("agc auto-squelch threshold : %12.8f dB\n", 20*log10f(_q->squelch_threshold));
#endif
    } else {
        // continuously increase threshold
        _q->squelch_threshold *= 1.01f;
    }
#endif
}

// execute squelch cycle
void AGC(_execute_squelch)(AGC() _q)
{
    // get signal level (linear rssi)
    T signal_level = AGC(_get_signal_level)(_q);

    int signal_low = (signal_level < _q->squelch_threshold) ? 1 : 0;

    switch (_q->squelch_status) {
        case LIQUID_AGC_SQUELCH_ENABLED:
            // update auto-squelch threshold
            if (_q->squelch_auto) AGC(_update_auto_squelch)(_q, signal_level);

            if (!signal_low) _q->squelch_status = LIQUID_AGC_SQUELCH_RISE;

#if AGC_SQUELCH_GAIN
            // actually squelch the input signal
            _q->g_squelch *= 0.92f;
#endif

            break;
        case LIQUID_AGC_SQUELCH_RISE:
#if AGC_SQUELCH_GAIN
            _q->g_squelch = 1.0f;
#endif
            _q->squelch_status = LIQUID_AGC_SQUELCH_SIGNALHI;
            break;
        case LIQUID_AGC_SQUELCH_SIGNALHI:
            if (signal_low) _q->squelch_status = LIQUID_AGC_SQUELCH_FALL;
            break;
        case LIQUID_AGC_SQUELCH_FALL:
            _q->squelch_status = LIQUID_AGC_SQUELCH_SIGNALLO;
            _q->squelch_timer = _q->squelch_timeout;
            break;
        case LIQUID_AGC_SQUELCH_SIGNALLO:
            if (!signal_low) {
                _q->squelch_status = LIQUID_AGC_SQUELCH_SIGNALHI;
            } else if (_q->squelch_timer > 0) {
                _q->squelch_timer--;
            } else {
                _q->squelch_status = LIQUID_AGC_SQUELCH_TIMEOUT;
            }
            break;
        case LIQUID_AGC_SQUELCH_TIMEOUT:
            _q->squelch_status = LIQUID_AGC_SQUELCH_ENABLED;
            //printf("squelch enabled\n");
            break;
        default:
            fprintf(stderr, "error: agc_%s_execute_squelch(), invalid squelch code: %d\n", EXTENSION_FULL, _q->squelch_status);
            exit(1);
    }
}

