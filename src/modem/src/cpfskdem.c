/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011, 2012, 2013 Joseph Gaeddert
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
// continuous phase frequency-shift keying demodulator
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "liquid.internal.h"

// cpfskdem
struct cpfskdem_s {
    // common
    unsigned int bps;       // bits per symbol
    unsigned int k;         // samples per symbol
    unsigned int m;         // filter delay (symbols)
    float        beta;      // filter bandwidth parameter
    float        h;         // modulation index
    int          type;      // filter type (e.g. LIQUID_CPFSK_SQUARE)

    // demodulator...
    // differentiator
    // linear PSK demodulator
    // polyphase filterbank for timing recovery
    // equalizer
};

// create cpfskdem object (frequency modulator)
//  _bps    :   bits per symbol, _bps > 0
//  _k      :   samples/symbol, _k > 1
//  _m      :   filter delay (symbols), _m > 0
//  _h      :   filter bandwidth parameter, _beta > 0
//  _h      :   modulation index, _h > 0
//  _type   :   filter type (e.g. LIQUID_CPFSK_SQUARE)
cpfskdem cpfskdem_create(unsigned int _bps,
                         unsigned int _k,
                         unsigned int _m,
                         float        _beta,
                         float        _h,
                         int          _type)
{
    // validate input
    if (_bps == 0) {
        fprintf(stderr,"error: cpfskdem_create(), bits/symbol must be greater than 0\n");
        exit(1);
    } else if (_k < 2) {
        fprintf(stderr,"error: cpfskdem_create(), samples/symbol must be greater than 2\n");
        exit(1);
    } else if (_m == 0) {
        fprintf(stderr,"error: cpfskdem_create(), filter delay must be greater than 0\n");
        exit(1);
    } else if (_beta <= 0.0f || _beta > 1.0f) {
        fprintf(stderr,"error: cpfskdem_create(), filter roll-off must be in (0,1]\n");
        exit(1);
    } else if (_h <= 0.0f) {
        fprintf(stderr,"error: cpfskdem_create(), modulation index must be greater than 0\n");
        exit(1);
    }

    // create main object memory
    cpfskdem q = (cpfskdem) malloc(sizeof(struct cpfskdem_s));

    // set basic internal properties
    q->bps  = _bps;
    q->k    = _k;
    q->m    = _m;
    q->beta = _beta;
    q->type = _type;

    // create object depending upon input type
    switch(q->type) {
    case LIQUID_CPFSK_SQUARE:
    case LIQUID_CPFSK_RCOS:
    case LIQUID_CPFSK_GMSK:
        break;
    }

    // reset modem object
    cpfskdem_reset(q);

    return q;
}


// destroy modem object
void cpfskdem_destroy(cpfskdem _q)
{
    // free main object memory
    free(_q);
}

// print modulation internals
void cpfskdem_print(cpfskdem _q)
{
    printf("cpfskdem:\n");
    printf("    k   :   %u\n", _q->k);
}

// reset modem object
void cpfskdem_reset(cpfskdem _q)
{
}

// demodulate array of samples
//  _q      :   continuous-phase frequency demodulator object
//  _y      :   input sample array [size: _n x 1]
//  _n      :   input sample array length
//  _s      :   output symbol array
//  _nw     :   number of output symbols written
void cpfskdem_demodulate(cpfskdem        _q,
                         float complex * _y,
                         unsigned int    _n,
                         unsigned int  * _s,
                         unsigned int  * _nw)
{
    *_nw = 0;
}

