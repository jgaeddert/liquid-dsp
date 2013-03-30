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
// continuous phase frequency-shift keying modulator
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "liquid.internal.h"

// cpfskmod
struct cpfskmod_s {
    // common
    unsigned int bps;           // bits per symbol
    unsigned int k;             // samples per symbol
    unsigned int m;             // filter delay (symbols)
    float        beta;          // filter bandwidth parameter
    float        h;             // modulation index
    int          type;          // filter type (e.g. LIQUID_CPFSK_SQUARE)

    // modulator
    interp_rrrf  interp;        // pulse-shaping filter interpolator
    iirfilt_rrrf integrator;    // phase integrator
};


// create cpfskmod object (frequency modulator)
//  _bps    :   bits per symbol, _bps > 0
//  _k      :   samples/symbol, _k > 1
//  _m      :   filter delay (symbols), _m > 0
//  _h      :   filter bandwidth parameter, _beta > 0
//  _h      :   modulation index, _h > 0
//  _type   :   filter type (e.g. LIQUID_CPFSK_SQUARE)
cpfskmod cpfskmod_create(unsigned int _bps,
                         unsigned int _k,
                         unsigned int _m,
                         float        _beta,
                         float        _h,
                         int          _type)
{
    // validate input
    if (_bps == 0) {
        fprintf(stderr,"error: cpfskmod_create(), bits/symbol must be greater than 0\n");
        exit(1);
    } else if (_k < 2) {
        fprintf(stderr,"error: cpfskmod_create(), samples/symbol must be greater than 2\n");
        exit(1);
    } else if (_m == 0) {
        fprintf(stderr,"error: cpfskmod_create(), filter delay must be greater than 0\n");
        exit(1);
    } else if (_beta <= 0.0f || _beta > 1.0f) {
        fprintf(stderr,"error: cpfskmod_create(), filter roll-off must be in (0,1]\n");
        exit(1);
    } else if (_h <= 0.0f) {
        fprintf(stderr,"error: cpfskmod_create(), modulation index must be greater than 0\n");
        exit(1);
    }

    // create main object memory
    cpfskmod q = (cpfskmod) malloc(sizeof(struct cpfskmod_s));

    // set basic internal properties
    q->bps  = _bps;
    q->k    = _k;
    q->m    = _m;
    q->beta = _beta;
    q->type = _type;

    // create object depending upon input type
    float b[2] = {0.5f,  0.5f};
    float a[2] = {1.0f, -1.0f};
    unsigned int h_len = 0;
    float * h = NULL;
    unsigned int i;
    switch(q->type) {
    case LIQUID_CPFSK_SQUARE:
    case LIQUID_CPFSK_RCOS:
    case LIQUID_CPFSK_GMSK:
        h_len = q->k;
        h = (float*) malloc(h_len*sizeof(float));
        for (i=0; i<h_len; i++)
            h[i] = q->h * M_PI / (float)(q->k);
        break;
    }

    // create pulse-shaping filter
    q->interp = interp_rrrf_create(q->k, h, h_len);
    free(h);

    // create phase integrator
    q->integrator = iirfilt_rrrf_create(b,2,a,2);

    // reset modem object
    cpfskmod_reset(q);

    return q;
}

// destroy cpfskmod object
void cpfskmod_destroy(cpfskmod _q)
{
    // destroy pulse-shaping filter/interpolator
    interp_rrrf_destroy(_q->interp);

    // destroy phase integrator
    iirfilt_rrrf_destroy(_q->integrator);

    // free main object memory
    free(_q);
}

// print cpfskmod object internals
void cpfskmod_print(cpfskmod _q)
{
}

// reset state
void cpfskmod_reset(cpfskmod _q)
{
}

// modulate sample
//  _q      :   frequency modulator object
//  _s      :   input symbol
//  _y      :   output sample array [size: _k x 1]
void cpfskmod_modulate(cpfskmod        _q,
                       unsigned int    _s,
                       float complex * _y)
{
}

