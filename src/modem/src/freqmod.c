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
#include <stdint.h>
#include <math.h>

#include "liquid.internal.h"

// freqmod
struct FREQMOD(_s) {
    float kf;   // modulation factor for FM
    T     ref;  // phase reference: kf*2^16

    // look-up table
    unsigned int sincos_table_len;      // table length: 10 bits
    uint16_t     sincos_table_phase;    // accumulated phase: 16 bits
    TC *         sincos_table;          // sin|cos look-up table: 2^10 entries
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

    // set modulation factor
    q->kf  = _kf;
    q->ref = q->kf * (1<<16);

    // initialize look-up table
    q->sincos_table_len = 1024;
    q->sincos_table     = (TC*) malloc( q->sincos_table_len*sizeof(TC) );
    unsigned int i;
    for (i=0; i<q->sincos_table_len; i++) {
        q->sincos_table[i] = cexpf(_Complex_I*2*M_PI*(float)i / (float)(q->sincos_table_len) );
    }

    // reset modem object
    FREQMOD(_reset)(q);

    // return object
    return q;
}

// destroy modem object
void FREQMOD(_destroy)(FREQMOD() _q)
{
    // free table
    free(_q->sincos_table);

    // free main object memory
    free(_q);
}

// print modulation internals
void FREQMOD(_print)(FREQMOD() _q)
{
    printf("freqmod:\n");
    printf("    mod. factor         :   %8.4f\n", _q->kf);
    printf("    sincos table len    :   %u\n",    _q->sincos_table_len);
}

// reset modem object
void FREQMOD(_reset)(FREQMOD() _q)
{
    // reset phase accumulation
    _q->sincos_table_phase = 0;
}

// modulate sample
//  _q      :   frequency modulator object
//  _m      :   message signal m(t)
//  _s      :   complex baseband signal s(t)
void FREQMOD(_modulate)(FREQMOD()   _q,
                        T           _m,
                        TC *        _s)
{
    // accumulate phase; this wraps around a 16-bit boundary and ensures
    // that negative numbers are mapped to positive numbers
    _q->sincos_table_phase =
        (_q->sincos_table_phase + (1<<16) + (int)roundf(_q->ref*_m)) & 0xffff;

    // compute table index: mask out 10 most significant bits with rounding
    // (adding 0x0020 effectively rounds to nearest value with 10 bits of
    // precision)
    unsigned int index = ( (_q->sincos_table_phase+0x0020) >> 6) & 0x03ff;

    // return table value at index
    *_s = _q->sincos_table[index];
}

// modulate block of samples
//  _q      :   frequency modulator object
//  _m      :   message signal m(t), [size: _n x 1]
//  _n      :   number of input, output samples
//  _s      :   complex baseband signal s(t) [size: _n x 1]
void FREQMOD(_modulate_block)(FREQMOD()    _q,
                              T *          _m,
                              unsigned int _n,
                              TC *         _s)
{
    // TODO: implement more efficient method
    unsigned int i;
    for (i=0; i<_n; i++)
        FREQMOD(_modulate)(_q, _m[i], &_s[i]);
}

