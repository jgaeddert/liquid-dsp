/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
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
// M-ary frequency-shift keying modulator
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "liquid.internal.h"

// 
// internal methods
//

// fskmod
struct fskmod_s {
    // common
    unsigned int m;             // bits per symbol
    unsigned int k;             // samples per symbol
    float        bandwidth;     // filter bandwidth parameter

    unsigned int M;             // constellation size
    float        M2;            // (M-1)/2
    nco_crcf     oscillator;    // nco
};

// create fskmod object (frequency modulator)
//  _m          :   bits per symbol, _m > 0
//  _k          :   samples/symbol, _k >= 2^_m
//  _bandwidth  :   total signal bandwidth, (0,0.5)
fskmod fskmod_create(unsigned int _m,
                     unsigned int _k,
                     float        _bandwidth)
{
    // validate input
    if (_m == 0) {
        fprintf(stderr,"error: fskmod_create(), bits/symbol must be greater than 0\n");
        exit(1);
    } else if (_k < 2 || _k > 2048) {
        fprintf(stderr,"error: fskmod_create(), samples/symbol must be in [2^_m, 2048]\n");
        exit(1);
    } else if (_bandwidth <= 0.0f || _bandwidth >= 0.5f) {
        fprintf(stderr,"error: fskmod_create(), bandwidth must be in (0,0.5)\n");
        exit(1);
    }

    // create main object memory
    fskmod q = (fskmod) malloc(sizeof(struct fskmod_s));

    // set basic internal properties
    q->m         = _m;              // bits per symbol
    q->k         = _k;              // samples per symbol
    q->bandwidth = _bandwidth;      // signal bandwidth

    // derived values
    q->M  = 1 << q->m;              // constellation size
    q->M2 = 0.5f*(float)(q->M-1);   // (M-1)/2

    q->oscillator = nco_crcf_create(LIQUID_VCO);

    // reset modem object
    fskmod_reset(q);

    // return main object
    return q;
}

// destroy fskmod object
void fskmod_destroy(fskmod _q)
{
    // destroy oscillator object
    nco_crcf_destroy(_q->oscillator);

    // free main object memory
    free(_q);
}

// print fskmod object internals
void fskmod_print(fskmod _q)
{
    printf("fskmod : frequency-shift keying modulator\n");
    printf("    bits/symbol     :   %u\n", _q->m);
    printf("    samples/symbol  :   %u\n", _q->k);
    printf("    bandwidth       :   %8.5f\n", _q->bandwidth);
}

// reset state
void fskmod_reset(fskmod _q)
{
    // reset internal objects
    nco_crcf_reset(_q->oscillator);
}

// modulate sample
//  _q      :   frequency modulator object
//  _s      :   input symbol
//  _y      :   output sample array [size: _k x 1]
void fskmod_modulate(fskmod          _q,
                     unsigned int    _s,
                     float complex * _y)
{
    // validate input
    if (_s >= _q->M) {
        fprintf(stderr,"warning: fskmod_modulate(), input symbol (%u) exceeds maximum (%u)\n",
                _s, _q->M);
        _s = 0;
    }

    // compute appropriate frequency
    float dphi = ((float)_s - _q->M2) * 2 * M_PI * _q->bandwidth / _q->M2;

    // set frequency appropriately
    nco_crcf_set_frequency(_q->oscillator, dphi);

    // generate output tone
    unsigned int i;
    for (i=0; i<_q->k; i++) {
        // compute complex output
        nco_crcf_cexpf(_q->oscillator, &_y[i]);
        
        // step oscillator
        nco_crcf_step(_q->oscillator);
    }
}

