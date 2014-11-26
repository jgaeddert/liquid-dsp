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
// Generic channel
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

// portable structured channel object
struct CHANNEL(_s) {
    // additive white Gauss noise
    int enabled_awgn;   // AWGN enabled?
    T   gamma;          // channel gain
    T   nstd;           // noise standard deviation
    float noise_floor_dB;
    float SNRdB;

    // carrier offset
    int enabled_carrier;// carrier offset enabled?
    float dphi;         // channel gain
    float phi;          // noise standard deviation
    NCO() nco;          // oscillator
};

// create structured channel object
//  _h      :   coefficients array [size: 1 x _n]
//  _n      :   channel length
CHANNEL() CHANNEL(_create)(TC *         _h,
                           unsigned int _n)
{
    CHANNEL() q = (CHANNEL()) malloc(sizeof(struct CHANNEL(_s)));

    // initialize all options as off
    q->enabled_awgn     = 0;
    q->enabled_carrier  = 0;

    // create internal objects
    q->nco = NCO(_create)(LIQUID_VCO);

    // return object
    return q;
}

// destroy channel object
void CHANNEL(_destroy)(CHANNEL() _q)
{
    // destroy internal objects
    NCO(_destroy)(_q->nco);

    // free main object memory
    free(_q);
}

// print channel object
void CHANNEL(_print)(CHANNEL() _q)
{
    printf("channel\n");
    if (_q->enabled_awgn) {
        printf("  AWGN:\n");
    }
}

// apply additive white Gausss noise impairment
//  _q              : channel object
//  _noise_floor_dB : noise floor power spectral density
//  _SNR_dB         : signal-to-noise ratio [dB]
void CHANNEL(_add_awgn)(CHANNEL() _q,
                        float     _noise_floor_dB,
                        float     _SNRdB)
{
    // enable module
    _q->enabled_awgn = 1;

    //
    _q->noise_floor_dB = _noise_floor_dB;
    _q->SNRdB          = _SNRdB;

    // set values appropriately
    _q->nstd  = powf(10.0f, _noise_floor_dB/20.0f);
    _q->gamma = powf(10.0f, (_q->SNRdB+_q->noise_floor_dB)/20.0f);
}

// apply carrier offset impairment
//  _q          : channel object
//  _frequency  : carrier frequency offse [radians/sample
//  _phase      : carrier phase offset    [radians]
void CHANNEL(_add_carrier_offset)(CHANNEL() _q,
                                  float     _frequency,
                                  float     _phase)
{
    // enable module
    _q->enabled_carrier = 1;

    //
    _q->dphi    = _frequency;
    _q->phi     = _phase;

    // set values appropriately
    NCO(_set_frequency)(_q->nco, _q->dphi);
    NCO(_set_phase)(    _q->nco, _q->phi);
}

// apply channel impairments on input array
//  _q      : channel object
//  _x      : input array [size: _nx x 1]
//  _nx     : input array length
//  _y      : output array
//  _ny     : output array length
void CHANNEL(_execute)(CHANNEL()      _q,
                       TI *           _x,
                       unsigned int   _nx,
                       TO *           _y,
                       unsigned int * _ny)
{
    unsigned int i;

    for (i=0; i<_nx; i++) {

        // apply carrier if enabled
        if (_q->enabled_carrier)
            NCO(_mix_up)(_q->nco, _x[i], &_y[i]);
        else
            _y[i] = _x[i];

        // apply AWGN if enabled
        if (_q->enabled_awgn) {
            _y[i] *= _q->gamma;
            _y[i] += _q->nstd * ( randnf() + _Complex_I*randnf() ) * M_SQRT1_2;
        }
    }

    *_ny = _nx;
}

