/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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

#define AGC(name)           LIQUID_CONCAT(agc,name)
#define T                   float
#define TC                  float complex
#define FIR_FILTER(name)    LIQUID_CONCAT(fir_filter_rrrf,name)

struct AGC(_s) {
    T e;            // estimated signal energy
    T e_target;     // target signal energy

    // gain variables
    T g;            // current gain value
    T g_min;        // minimum gain value
    T g_max;        // maximum gain value

    // loop filter parameters
    T BT;           // bandwidth-time constant
    T alpha;        // feed-back gain
    T beta;         // feed-forward gain

    // loop filter state variables
    T e_prime;
    T e_hat;        // filtered energy estimate
    T tmp2;

    FIR_FILTER() f;
};


AGC() AGC(_create)(T _etarget, T _BT)
{
    AGC() _q = (AGC()) malloc(sizeof(struct AGC(_s)));
    AGC(_init)(_q);
    AGC(_set_target)(_q, _etarget);
    AGC(_set_bandwidth)(_q, _BT);

    // normalized windowing function
    T w[11] = {
       0.014792,
       0.042634,
       0.081587,
       0.122933,
       0.154722,
       0.166667,
       0.154722,
       0.122933,
       0.081587,
       0.042634,
       0.014792
    };

    unsigned int i;
    //for (i=0; i<11; i++)
    //    printf("w(%4u) = %8.4f;\n", i+1, w[i]);

    _q->f = FIR_FILTER(_create)(w,11);

    for (i=0; i<11; i++)
        FIR_FILTER(_push)(_q->f, 0.0f);

    return _q;
}

void AGC(_destroy)(AGC() _q)
{
    fir_filter_rrrf_destroy(_q->f);
    free(_q);
}

void AGC(_print)(AGC() _q)
{
    printf("agc [rssi: %12.4fdB]:\n", 10*log10(_q->e_target / _q->g));
}

void AGC(_reset)(AGC() _q)
{
    fir_filter_rrrf_clear(_q->f);
    _q->e_prime = 1.0f;
    _q->e_hat = 1.0f;
    _q->tmp2 = 1.0f;
}

void AGC(_init)(AGC() _q)
{
    //_q->e = 1.0f;
    _q->e_target = 1.0f;

    // set gain variables
    _q->g = 1.0f;
    _q->g_min = 1e-6f;
    _q->g_max = 1e+6f;

    // prototype loop filter
    AGC(_set_bandwidth)(_q, 0.01f);

    // initialize loop filter state variables
    _q->e_prime = 1.0f;
    _q->e_hat = 1.0f;
    _q->tmp2 = 1.0f;
}

void AGC(_set_target)(AGC() _q, T _e_target)
{
    // check to ensure _e_target is reasonable

    _q->e_target = _e_target;

    ///\todo auto-adjust gain to compensate?
}

void AGC(_set_gain_limits)(AGC() _q, T _g_min, T _g_max)
{
    if (_g_min > _g_max) {
        printf("error: agc_set_gain_limits(), _g_min < _g_max\n");
        exit(0);
    }

    _q->g_min = _g_min;
    _q->g_max = _g_max;
}

void AGC(_set_bandwidth)(AGC() _q, T _BT)
{
    // check to ensure _BT is reasonable
    if ( _BT <= 0 ) {
        perror("\n");
        exit(-1);
    } else if ( _BT > 0.5f ) {
        perror("\n");
        exit(-1);
    }

    _q->BT = _BT;
    _q->alpha = sqrtf(_q->BT);
    _q->beta = 1 - _q->alpha;
}

void AGC(_execute)(AGC() _q, TC _x, TC *_y)
{
    // estimate normalized energy, should be equal to 1.0 when locked
    T e2 = crealf(_x * conj(_x)); // NOTE: crealf used for roundoff error
    fir_filter_rrrf_push(_q->f, e2);
    T e_hat;
    fir_filter_rrrf_execute(_q->f, &e_hat);
    e_hat = sqrtf(e_hat);// * (_q->g) / (_q->e_target);

    // ideal gain
    T g = _q->e_target / e_hat;

    // accumulated gain
    _q->g = (_q->beta)*(_q->g) + (_q->alpha)*g;
    //_q->g = g;

    // limit gain
    if ( _q->g > _q->g_max )
        _q->g = _q->g_max;
    else if ( _q->g < _q->g_min )
        _q->g = _q->g_min;

    // apply gain to input
    *_y = _x * _q->g;
}

T AGC(_get_signal_level)(AGC() _q)
{
    return (_q->e_target / _q->g);
}

T AGC(_get_gain)(AGC() _q)
{
    return _q->g;
}

