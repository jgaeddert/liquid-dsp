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
    float kf;

    // frequency modulation phase integral
    float theta_prime;
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
    FREQMOD() q = (freqmod) malloc(sizeof(struct FREQMOD(_s)));

    // set basic internal properties
    q->kf   = _kf;      // modulation factor

    // reset modem object
    FREQMOD(_reset)(q);

    // return object
    return q;
}

// destroy modem object
void FREQMOD(_destroy)(FREQMOD() _q)
{
    // free main object memory
    free(_q);
}

// print modulation internals
void FREQMOD(_print)(FREQMOD() _q)
{
    printf("freqmod:\n");
    printf("    mod. factor :   %8.4f\n", _q->kf);
}

// reset modem object
void FREQMOD(_reset)(FREQMOD() _q)
{
    // reset integral
    _q->theta_prime = 0;
}

// modulate sample
//  _q      :   frequency modulator object
//  _m      :   message signal m(t)
//  _s      :   complex baseband signal s(t)
void FREQMOD(_modulate)(FREQMOD()   _q,
                        T           _m,
                        TC *        _s)
{
    // integrate result
    _q->theta_prime += 2 * M_PI * _q->kf * _m;

    // return complex exponential of integrated phase
    *_s = cexpf(_Complex_I*_q->theta_prime);
}

