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
// Frequency modulator/demodulator
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "liquid.internal.h"

// freqdem
struct FREQDEM(_s) {
    // common
    float kf;           // modulation index

    // derived values
    float twopikf_inv;  // 1/(2*pi*kf)
    float dphi;         // carrier frequency [radians]

    // demodulator
    float complex q;    // phase difference
};

// create freqdem object
//  _kf     :   modulation factor
FREQDEM() FREQDEM(_create)(float _kf)
{
    // validate input
    if (_kf <= 0.0f || _kf > 1.0) {
        fprintf(stderr,"error: freqdem_create(), modulation factor %12.4e out of range [0,1]\n", _kf);
        exit(1);
    }

    // create main object memory
    FREQDEM() q = (freqdem) malloc(sizeof(struct FREQDEM(_s)));

    // set basic internal properties
    q->kf   = _kf;      // modulation factor

    // compute derived values
    q->twopikf_inv = 1.0f / (2*M_PI*q->kf);       // 1 / (2*pi*kf)

    // reset modem object
    FREQDEM(_reset)(q);

    return q;
}

// destroy modem object
void FREQDEM(_destroy)(FREQDEM() _q)
{
    // free main object memory
    free(_q);
}

// print modulation internals
void FREQDEM(_print)(FREQDEM() _q)
{
    printf("freqdem:\n");
    printf("    mod. factor :   %8.4f\n", _q->kf);
}

// reset modem object
void FREQDEM(_reset)(FREQDEM() _q)
{
    // reset carrier frequency
    _q->dphi = 0.0f;

    // clear complex phase term
    _q->q = 0.0f;
}

// demodulate sample
//  _q      :   FM demodulator object
//  _r      :   received signal
//  _m      :   output message signal
void FREQDEM(_demodulate)(FREQDEM() _q,
                          TC        _r,
                          T *       _m)
{
    // compute phase difference and normalize by modulation index
    *_m = (cargf(conjf(_q->q)*(_r)) - _q->dphi) * _q->twopikf_inv;

    // save previous input sample
    _q->q = _r;
}


