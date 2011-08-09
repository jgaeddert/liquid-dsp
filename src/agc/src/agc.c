/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
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

// signal level estimate feedback
#define LIQUID_AGC_ZETA (0.5)

// agc structure object
struct AGC(_s) {
    // automatic gain control type
    liquid_agc_type type;

    T e_target;     // target signal energy

    // gain variables
    T g;            // current gain value
    T g_min;        // minimum gain value
    T g_max;        // maximum gain value

    // gain control loop filter parameters
    T BT;           // bandwidth-time constant
    T alpha;        // feed-back gain
    T beta;         // feed-forward gain

    // signal energy estimate iir filter state variables
    T e;            // instantaneous estimated signal energy
    T e_hat;        // filtered (average) energy estimate
    T e_prime;      // previous energy estimate

    // is agc locked?
    int is_locked;
    unsigned int decim_timer;
    unsigned int decim_timeout;

    // 'true' agc method
    float * x2;         // buffered x^2 values (LIQUID_AGC_TRUE)
    float x2_sum;       // accumulated sum of buffer
    unsigned int nx2;   // size of x^2 buffer
    unsigned int ix2;   // write index of x^2 buffer
    float sqrt_nx2;     // sqrt(nx2)

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
    _q->type = LIQUID_AGC_DEFAULT;

    // initialize loop filter state variables
    _q->e_prime = 1.0f;
    _q->e_hat = 1.0f;

    // set default gain variables
    _q->g = 1.0f;
    _q->g_min = 1e-6f;
    _q->g_max = 1e+6f;

    AGC(_set_target)(_q, 1.0);
    AGC(_set_bandwidth)(_q, 0.0);

    _q->is_locked = 0;
    _q->decim_timer = 0;
    AGC(_set_decim)(_q, 1);

    // create x^2 buffer, initialize with zeros
    _q->nx2 = 16;
    _q->sqrt_nx2 = sqrtf(_q->nx2);
    _q->x2 = (float*) malloc((_q->nx2)*sizeof(float));
    _q->ix2 = 0;
    _q->x2_sum = 0.0;
    unsigned int i;
    for (i=0; i<_q->nx2; i++)
        _q->x2[i] = 0.0f;

    // squelch
    _q->squelch_headroom = 0.39811f;    // roughly 4dB
    AGC(_squelch_disable_auto)(_q);
    AGC(_squelch_set_threshold)(_q, -30.0f);
    AGC(_squelch_set_timeout)(_q, 32);
    AGC(_squelch_deactivate)(_q);

    // return object
    return _q;
}

// destroy agc object, freeing all internally-allocated memory
void AGC(_destroy)(AGC() _q)
{
    free(_q->x2);
    free(_q);
}

// print agc object internals
void AGC(_print)(AGC() _q)
{
    printf("agc [rssi: %12.4fdB]:\n", 10*log10(_q->e_target / _q->g));
}

// reset agc object
void AGC(_reset)(AGC() _q)
{
    _q->e_prime = 1.0f;
    _q->e_hat   = 1.0f;
    _q->g       = 1.0f;

    _q->ix2 = 0;
    _q->x2_sum = 0.0;
    unsigned int i;
    for (i=0; i<_q->nx2; i++)
        _q->x2[i] = 0.0f;

    AGC(_unlock)(_q);
}

// set agc type
//  _q      :   agc object
//  _type   :   gain update type (e.g. LIQUID_AGC_DEFAULT)
void AGC(_set_type)(AGC() _q,
                    liquid_agc_type _type)
{
    _q->type = _type;
}

// set agc target signal level
//  _q          :   agc object
//  _e_target   :   target signal level (RMS)
void AGC(_set_target)(AGC() _q,
                      T _e_target)
{
    // validate input; check to ensure _e_target is reasonable
    if (_e_target <= 0.0f) {
        fprintf(stderr,"error: agc_xxxt_set_target(), target energy must be greater than 0\n");
        exit(-1);
    }

    _q->e_target = _e_target;
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
        fprintf(stderr,"error: agc_xxxt_set_gain_limits(), _g_min < _g_max\n");
        exit(-1);
    }

    _q->g_min = _g_min;
    _q->g_max = _g_max;
}

// set agc loop bandwidth
//  _q      :   agc object
//  _BT     :   bandwidth
void AGC(_set_bandwidth)(AGC() _q,
                         T _BT)
{
    // check to ensure _BT is reasonable
    if ( _BT < 0 ) {
        fprintf(stderr,"error: agc_xxxt_set_bandwidth(), bandwidth must be positive\n");
        exit(-1);
    } else if ( _BT > 1.0f ) {
        fprintf(stderr,"error: agc_xxxt_set_bandwidth(), bandwidth must less than 1.0\n");
        exit(-1);
    }

    // set internal bandwidth
    _q->BT = _BT;

    // normalize bandwidth by decimation factor
    float bt = _q->BT * _q->decim_timeout;

    // ensure normalized bandwidth is less than one
    if (bt >= 1.0f) bt = 0.99f;

    // compute coefficients
    _q->alpha = sqrtf(bt);
    _q->beta = 1 - _q->alpha;
}

// Set internal decimation level
//  _q      :   agc object
//  _D      :   decimation level, D > 0, D=4 typical
void AGC(_set_decim)(AGC() _q,
                     unsigned int _D)
{
    // validate input
    if ( _D == 0 ) {
        fprintf(stderr,"error: agc_xxxt_set_decim(), decimation factor must be greater than zero\n");
        exit(1);
    }

    // set decimation factor
    _q->decim_timeout = _D;

    // re-compute filter bandwidth
    AGC(_set_bandwidth)(_q, _q->BT);
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

// execute automatic gain control loop
//  _q      :   agc object
//  _x      :   input sample
//  _y      :   output sample
void AGC(_execute)(AGC() _q,
                   TC _x,
                   TC *_y)
{
    // if agc is locked, apply current gain and return
    if (_q->is_locked) {
        *_y = _x * (_q->g);
        return;
    }

    // decimation: only execute gain control loop every D samples
    _q->decim_timer++;
    if (_q->decim_timer == _q->decim_timeout) {
        _q->decim_timer = 0;
    } else {
        // apply gain to input and return
        *_y = _x * _q->g;
        return;
    }

    // execute type-specific gain estimation
    switch (_q->type) {
    case LIQUID_AGC_DEFAULT:
        AGC(_estimate_gain_default)(_q,_x);
        break;
    case LIQUID_AGC_LOG:
        AGC(_estimate_gain_log)(_q,_x);
        break;
    case LIQUID_AGC_EXP:
        AGC(_estimate_gain_exp)(_q,_x);
        break;
    case LIQUID_AGC_TRUE:
        AGC(_estimate_gain_true)(_q,_x);
        break;
    default:
        // should never get to this condition
        fprintf(stderr,"error: agc_xxxt_execute(), invalid agc type\n");
        exit(-1);
    }

    // limit gain
    AGC(_limit_gain)(_q);

    // apply gain to input
    *_y = _x * _q->g;

    // update squelch control, if activated
    if (_q->squelch_activated)
        AGC(_execute_squelch)(_q);
}

// get estimated signal level
T AGC(_get_signal_level)(AGC() _q)
{
    return (_q->e_target / _q->g);
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
                                 T _threshold)
{
    _q->squelch_threshold      = powf(10.0f,_threshold / 10.0f);
    _q->squelch_threshold_auto = _q->squelch_threshold;
}

// get squelch threshold [dB]
T AGC(_squelch_get_threshold)(AGC() _q)
{
    return 10.0f*log10f(_q->squelch_threshold);
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
                                 TC _x)
{
    // compute instantaneous signal energy
#if TC_COMPLEX
    //_q->e = crealf(_x * conj(_x)); // NOTE: crealf used for roundoff error
    // same as above, but faster since we are throwing away imaginary component
    _q->e = crealf(_x)*crealf(_x) + cimagf(_x)*cimagf(_x);
#else
    _q->e = _x*_x;
#endif

    // filter energy estimate
    _q->e_prime = (_q->e)*LIQUID_AGC_ZETA + (_q->e_prime)*(1.0 - LIQUID_AGC_ZETA);
    _q->e_hat = sqrtf(_q->e_prime);
}

// estimate necessary agc gain (default method)
//  _q      :   agc object
//  _x      :   input sample
void AGC(_estimate_gain_default)(AGC() _q,
                                 TC _x)
{
    // estimate input energy, result is in _q->e_hat
    AGC(_estimate_input_energy)(_q, _x);

    // ideal gain
    T g = _q->e_target / _q->e_hat;

    // accumulated gain
    _q->g = (_q->beta)*(_q->g) + (_q->alpha)*g;
}

// estimate necessary agc gain (logarithmic method)
//  _q      :   agc object
//  _x      :   input sample
void AGC(_estimate_gain_log)(AGC() _q,
                             TC _x)
{
    // estimate input energy, result is in _q->e_hat
    AGC(_estimate_input_energy)(_q, _x);

    // loop filter : compute gain error
    T gain_error = _q->e_target / (_q->e_hat * _q->g);

    // adjust gain proportional to log of error
    _q->g *= powf(gain_error, _q->alpha);
}

// estimate necessary agc gain (exponential method)
//  _q      :   agc object
//  _x      :   input sample
void AGC(_estimate_gain_exp)(AGC() _q,
                             TC _x)
{
    // estimate input energy, result is in _q->e_hat
    AGC(_estimate_input_energy)(_q, _x);

    // compute estimate of output signal level
    T e_out = _q->e_hat * _q->g;

    if (e_out > _q->e_target) {
        // decrease gain proportional to energy difference
        _q->g *= 1.0f - _q->alpha * (e_out - _q->e_target) / e_out;
    } else {
        // increase gain proportional to energy difference
        _q->g *= 1.0f - _q->alpha * (e_out - _q->e_target) / _q->e_target;
    }
}

// estimate necessary agc gain ('true' method)
//  _q      :   agc object
//  _x      :   input sample
void AGC(_estimate_gain_true)(AGC() _q,
                              TC _x)
{
    // compute |_x|^2
    //float x2 = creal(_x*conj(_x));
    float x2 = creal(_x)*creal(_x) + cimag(_x)*cimag(_x);

    // increment sum by _x^2
    _q->x2_sum += x2;

    // decrement sum by buffer value
    _q->x2_sum -= _q->x2[ _q->ix2 ];

    // push sample into buffer
    _q->x2[_q->ix2] = x2;

    // increment index
    _q->ix2 = (_q->ix2 + 1) % _q->nx2;

    // set gain accordingly
    _q->g = _q->sqrt_nx2 / sqrtf(_q->x2_sum + 1e-12f);
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
                               T _rssi)
{
    // if rssi dips too low (roughly 4dB below threshold),
    // decrease threshold slightly
    if (_rssi < _q->squelch_threshold * _q->squelch_headroom) {
        _q->squelch_threshold *= 0.95f;
#if 0
        printf("agc auto-squelch threshold : %12.8f dB\n", 10*log10f(_q->squelch_threshold));
#endif
    } else {
        // continuously increase threshold
        _q->squelch_threshold *= 1.01f;
    }
}

// execute squelch cycle
void AGC(_execute_squelch)(AGC() _q)
{
    // get rssi
    T rssi = AGC(_get_signal_level)(_q);

    int signal_low = (rssi < _q->squelch_threshold) ? 1 : 0;

    switch (_q->squelch_status) {
        case LIQUID_AGC_SQUELCH_ENABLED:
            // update auto-squelch threshold
            if (_q->squelch_auto) AGC(_update_auto_squelch)(_q, rssi);

            if (!signal_low) _q->squelch_status = LIQUID_AGC_SQUELCH_RISE;
            break;
        case LIQUID_AGC_SQUELCH_RISE:
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
            fprintf(stderr, "error: agc_xxxt_execute_squelch(), invalid squelch code: %d\n", _q->squelch_status);
            exit(1);
    }
}

