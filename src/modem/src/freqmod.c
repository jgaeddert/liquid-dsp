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
// Frequency modulator
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "liquid.internal.h"

// freqmod
struct FREQMOD(_s) {
    // modulation factor for FM
    T kf;

    // audio prefilter
    IIRFILT_RRR() prefilter;

    // frequency modulation phase integrator
    IIRFILT_RRR() integrator;
};

// create freqmod object
//  _kf     :   modulation factor
FREQMOD() FREQMOD(_create)(float _kf)
{
    // validate input
    if (_kf <= 0.0f || _kf > 1.0) {
        fprintf(stderr,"error: freqmod_create(), modulation factor %12.4e out of range [0,1]\n", _kf);
        exit(1);
    }

    // create main object memory
    FREQMOD() q = (FREQMOD()) malloc(sizeof(struct FREQMOD(_s)));

    // set basic internal properties
    // modulation factor
#if LIQUID_FPM
    q->kf = Q(_float_to_fixed)(_kf);
#else
    q->kf = _kf;
#endif

    // create modulator objects
#if defined LIQUID_FPM
    T b[2] = {Q(_float_to_fixed)(0.5f),Q(_float_to_fixed)( 0.5f)};
    T a[2] = {Q(_float_to_fixed)(1.0f),Q(_float_to_fixed)(-1.0f)};
#else
    float b[2] = {0.5f,  0.5f};
    float a[2] = {1.0f, -1.0f};
#endif
    q->integrator = IIRFILT_RRR(_create)(b,2,a,2);

    // create prefilter (block DC values)
    q->prefilter = IIRFILT_RRR(_create_dc_blocker)(5e-4f);

    // reset modem object
    FREQMOD(_reset)(q);

    return q;
}

// destroy modem object
void FREQMOD(_destroy)(FREQMOD() _q)
{
    // destroy audio pre-filter
    IIRFILT_RRR(_destroy)(_q->prefilter);

    // destroy integrator
    IIRFILT_RRR(_destroy)(_q->integrator);

    // free main object memory
    free(_q);
}

// print modulation internals
void FREQMOD(_print)(FREQMOD() _q)
{
    printf("freqmod:\n");
#if LIQUID_FPM
    printf("    mod. factor :   %8.4f\n", Q(_fixed_to_float)(_q->kf));
#else
    printf("    mod. factor :   %8.4f\n", _q->kf);
#endif
}

// reset modem object
void FREQMOD(_reset)(FREQMOD() _q)
{
    // reset audio pre-filter
    IIRFILT_RRR(_reset)(_q->prefilter);

    // reset integrator object
    IIRFILT_RRR(_reset)(_q->integrator);
}

// modulate sample
//  _q      :   frequency modulator object
//  _m      :   message signal m(t)
//  _s      :   complex baseband signal s(t)
void FREQMOD(_modulate)(FREQMOD() _q,
                        T         _m,
                        TC *      _s)
{
    // push sample through pre-filter
    IIRFILT_RRR(_execute)(_q->prefilter, _m, &_m);
    
    // integrate result
#if LIQUID_FPM
    // TODO: implement better fixed-point version
    // integrate phase
    q16_t theta_i = 0;
    IIRFILT_RRR(_execute)(_q->integrator, Q(_mul)(_q->kf,_m), &theta_i);

    // compute complex exponential
    *_s = CQ(_cexpj)( Q(_mul)(Q(_2pi),theta_i) );
#else
    float theta_i = 0.0f;
    IIRFILT_RRR(_execute)(_q->integrator, _q->kf*_m, &theta_i);
    *_s = cexpf(_Complex_I*2.0f*M_PI*theta_i);
#endif
}

