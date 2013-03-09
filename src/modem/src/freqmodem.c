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
// Frequency modulator/demodulator
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "liquid.internal.h"

// freqmodem
struct freqmodem_s {
    // common
    float fc;                   // carrier frequency, range: [-0.5,0.5]
    float kf;                   // modulation index

    // derived values
    float kf_inv;               // 1/kf
    float dphi;                 // carrier frequency [radians]

    // demodulator
    liquid_freqmodem_type type; // demodulator type (PLL, DELAYCONJ)
    nco_crcf oscillator;        // nco
    float complex q;            // phase difference
};

// create freqmodem object
//  _kf     :   modulation factor
//  _fc     :   carrier frequency, fc in [-0.5, 0.5]
//  _type   :   demodulation type (e.g. LIQUID_FREQMODEM_DELAYCONJ)
freqmodem freqmodem_create(float                 _kf,
                           float                 _fc,
                           liquid_freqmodem_type _type)
{
    // validate input
    if (_kf <= 0.0f || _kf > 1.0) {
        fprintf(stderr,"error: freqmodem_create(), modulation factor %12.4e out of range [0,1]\n", _kf);
        exit(1);
    } else if (_fc < -0.5f || _fc > 0.5f) {
        fprintf(stderr,"error: freqmodem_create(), carrier frequency %12.4e out of range [-0.5,0.5]\n", _fc);
        exit(1);
    }

    // create main object memory
    freqmodem q = (freqmodem) malloc(sizeof(struct freqmodem_s));

    // set basic internal properties
    q->type = _type;    // demod type
    q->kf   = _kf;      // modulation factor
    q->fc   = _fc;      // carrier frequency

    // compute derived values
    q->kf_inv = 1.0f / q->kf;       // 1 / kf
    q->dphi   = q->fc * 2 * M_PI;   // 

    // create oscillator
    q->oscillator = nco_crcf_create(LIQUID_VCO);

    //
    if (q->type == LIQUID_FREQMODEM_PLL) {
        // TODO : set initial NCO frequency ?
        // create phase-locked loop
        nco_crcf_pll_set_bandwidth(q->oscillator, 0.05f);
    }

    // reset modem object
    freqmodem_reset(q);

    return q;
}

// destroy modem object
void freqmodem_destroy(freqmodem _q)
{
    nco_crcf_destroy(_q->oscillator);
    free(_q);
}

// print modulation internals
void freqmodem_print(freqmodem _q)
{
    printf("freqmodem:\n");
    printf("    mod. factor :   %8.4f\n", _q->kf);
    printf("    fc          :   %8.4f\n", _q->fc);
}

// reset modem object
void freqmodem_reset(freqmodem _q)
{
    // reset oscillator, phase-locked loop
    nco_crcf_reset(_q->oscillator);

    // clear complex phase term
    _q->q = 0.0f;
}

// modulate input sample
void freqmodem_modulate(freqmodem       _q,
                        float           _x,
                        float complex * _y)
{
    nco_crcf_set_frequency(_q->oscillator,
                          (_q->kf)*_x + _q->dphi);

    nco_crcf_cexpf(_q->oscillator, _y);
    nco_crcf_step(_q->oscillator);
}

// run demodulator
void freqmodem_demodulate(freqmodem     _q,
                          float complex _y,
                          float *       _x)
{
    if (_q->type == LIQUID_FREQMODEM_PLL) {
        // 
        // push through phase-locked loop
        //

        // compute phase error from internal NCO complex exponential
        float complex p;
        nco_crcf_cexpf(_q->oscillator, &p);
        float phase_error = cargf( conjf(p)*_y );

        // step the PLL and the internal NCO object
        nco_crcf_pll_step(_q->oscillator, phase_error);
        nco_crcf_step(_q->oscillator);

        // demodulated signal is (weighted) nco frequency
        *_x = (nco_crcf_get_frequency(_q->oscillator) -_q->dphi) * _q->kf_inv;
    } else {
        // compute phase difference and normalize by modulation index
        *_x = (cargf(conjf(_q->q)*(_y)) - _q->dphi) * _q->kf_inv;

        _q->q = _y;
    }
}


