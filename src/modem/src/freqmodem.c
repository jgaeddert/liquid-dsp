/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
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
    liquid_freqmodem_type type; // demodulator type (PLL, DELAYCONJ)
    float fc;                   // carrier frequency, range: [-0.5,0.5]
    float m;                    // modulation index

    // derived values
    float m_inv;                // 1/m
    float dphi;                 // carrier frequency [radians]

    nco_crcf oscillator;        // nco
    float complex q;            // phase difference
};

// create freqmodem object
//  _m      :   modulation index
//  _fc     :   carrier frequency, -0.5 <= _fc < 0.5
//  _type   :   demodulation type (e.g. LIQUID_FREQMODEM_DELAYCONJ)
freqmodem freqmodem_create(float _m,
                           float _fc,
                           liquid_freqmodem_type _type)
{
    freqmodem fm = (freqmodem) malloc(sizeof(struct freqmodem_s));
    fm->type = _type;
    fm->m = _m;
    fm->fc = _fc;
    if (fm->m <= 0.0f || fm->m > 2.0f*M_PI) {
        fprintf(stderr,"error: freqmodem_create(), modulation index %12.4e out of range (0,2*pi)\n", fm->m);
        exit(1);
    } else if (fm->fc <= -0.5f || fm->fc >= 0.5f) {
        fprintf(stderr,"error: freqmodem_create(), carrier frequency %12.4e out of range (-0.5,0.5)\n", fm->fc);
        exit(1);
    }

    // compute derived values
    fm->m_inv = 1.0f / fm->m;
    fm->dphi  = fm->fc * 2 * M_PI;

    // create oscillator
    fm->oscillator = nco_crcf_create(LIQUID_VCO);

    if (fm->type == LIQUID_FREQMODEM_PLL) {
        // TODO : set initial NCO frequency ?
        // create phase-locked loop
        nco_crcf_pll_set_bandwidth(fm->oscillator, 0.05f);
    }

    freqmodem_reset(fm);

    return fm;
}

void freqmodem_destroy(freqmodem _fm)
{
    nco_crcf_destroy(_fm->oscillator);
    free(_fm);
}

void freqmodem_print(freqmodem _fm)
{
    printf("freqmodem:\n");
    printf("    mod. index  :   %8.4f\n", _fm->m);
    printf("    fc          :   %8.4f\n", _fm->fc);
}

void freqmodem_reset(freqmodem _fm)
{
    // reset oscillator, phase-locked loop
    nco_crcf_reset(_fm->oscillator);

    // clear complex phase term
    _fm->q = 0.0f;
}

void freqmodem_modulate(freqmodem _fm,
                        float _x,
                        float complex *_y)
{
    nco_crcf_set_frequency(_fm->oscillator,
                      (_fm->m)*_x + _fm->dphi);

    nco_crcf_cexpf(_fm->oscillator, _y);
    nco_crcf_step(_fm->oscillator);
}

void freqmodem_demodulate(freqmodem _fm,
                          float complex _y,
                          float *_x)
{
    if (_fm->type == LIQUID_FREQMODEM_PLL) {
        // 
        // push through phase-locked loop
        //

        // compute phase error from internal NCO complex exponential
        float complex p;
        nco_crcf_cexpf(_fm->oscillator, &p);
        float phase_error = cargf( conjf(p)*_y );

        // step the PLL and the internal NCO object
        nco_crcf_pll_step(_fm->oscillator, phase_error);
        nco_crcf_step(_fm->oscillator);

        // demodulated signal is (weighted) nco frequency
        *_x = (nco_crcf_get_frequency(_fm->oscillator) -_fm->dphi) * _fm->m_inv;
    } else {
        // compute phase difference and normalize by modulation index
        *_x = (cargf(conjf(_fm->q)*(_y)) - _fm->dphi) * _fm->m_inv;

        _fm->q = _y;
    }
}


