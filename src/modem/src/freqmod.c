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
// Frequency modulator
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "liquid.internal.h"

// freqmod
struct freqmod_s {
    // modulation factor for FM
    float kf;

    // audio prefilter
    iirfilt_rrrf prefilter;

    // frequency modulation phase integrator
    iirfilt_rrrf integrator;
};

// create freqmod object
//  _kf     :   modulation factor
freqmod freqmod_create(float _kf)
{
    // validate input
    if (_kf <= 0.0f || _kf > 1.0) {
        fprintf(stderr,"error: freqmod_create(), modulation factor %12.4e out of range [0,1]\n", _kf);
        exit(1);
    }

    // create main object memory
    freqmod q = (freqmod) malloc(sizeof(struct freqmod_s));

    // set basic internal properties
    q->kf   = _kf;      // modulation factor

    // create modulator objects
    q->integrator = iirfilt_rrrf_create_integrator();

    // create prefilter (block DC values)
    float b[2] = {1.0f, -1.0f  };
    float a[2] = {1.0f, -0.9995f};
    q->prefilter = iirfilt_rrrf_create(b, 2, a, 2);

    // reset modem object
    freqmod_reset(q);

    return q;
}

// destroy modem object
void freqmod_destroy(freqmod _q)
{
    // destroy audio pre-filter
    iirfilt_rrrf_destroy(_q->prefilter);

    // destroy integrator
    iirfilt_rrrf_destroy(_q->integrator);

    // free main object memory
    free(_q);
}

// print modulation internals
void freqmod_print(freqmod _q)
{
    printf("freqmod:\n");
    printf("    mod. factor :   %8.4f\n", _q->kf);
}

// reset modem object
void freqmod_reset(freqmod _q)
{
    // reset audio pre-filter
    iirfilt_rrrf_clear(_q->prefilter);

    // reset integrator object
    iirfilt_rrrf_clear(_q->integrator);
}

// modulate sample
//  _q      :   frequency modulator object
//  _m      :   message signal m(t)
//  _s      :   complex baseband signal s(t)
void freqmod_modulate(freqmod         _q,
                      float           _m,
                      float complex * _s)
{
    // push sample through pre-filter
    iirfilt_rrrf_execute(_q->prefilter, _m, &_m);
    
    // integrate result
    float theta_i = 0.0f;
    iirfilt_rrrf_execute(_q->integrator, _q->kf*_m, &theta_i);
    *_s = cexpf(_Complex_I*2.0f*M_PI*theta_i);
}

