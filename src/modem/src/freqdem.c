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

// freqdem
struct freqdem_s {
    // common
    float kf;                   // modulation index

    // derived values
    float twopikf_inv;          // 1/(2*pi*kf)
    float dphi;                 // carrier frequency [radians]

    // demodulator
    liquid_freqdem_type type;   // demodulator type (PLL, DELAYCONJ)
    nco_crcf oscillator;        // nco
    float complex q;            // phase difference
    iirfilt_rrrf postfilter;    // post-filter
};

// create freqdem object
//  _kf     :   modulation factor
//  _type   :   demodulation type (e.g. LIQUID_FREQDEM_DELAYCONJ)
freqdem freqdem_create(float                 _kf,
                           liquid_freqdem_type _type)
{
    // validate input
    if (_kf <= 0.0f || _kf > 1.0) {
        fprintf(stderr,"error: freqdem_create(), modulation factor %12.4e out of range [0,1]\n", _kf);
        exit(1);
    }

    // create main object memory
    freqdem q = (freqdem) malloc(sizeof(struct freqdem_s));

    // set basic internal properties
    q->type = _type;    // demod type
    q->kf   = _kf;      // modulation factor

    // compute derived values
    q->twopikf_inv = 1.0f / (2*M_PI*q->kf);       // 1 / (2*pi*kf)

    // create oscillator
    q->oscillator = nco_crcf_create(LIQUID_VCO);

    // create post-filter
    float alpha = 0.999f;
    float beta  = 0.990f;
    float B[3] = { 1.0f, -1.0f,         0.0f       };
    float A[3] = { 1.0f, -(alpha+beta), alpha*beta };
    q->postfilter = iirfilt_crcf_create_sos(B, A, 1);

    //
    if (q->type == LIQUID_FREQDEM_PLL) {
        // TODO : set initial NCO frequency ?
        // create phase-locked loop
        nco_crcf_pll_set_bandwidth(q->oscillator, 0.05f);
    }

    // reset modem object
    freqdem_reset(q);

    return q;
}

// destroy modem object
void freqdem_destroy(freqdem _q)
{
    // destroy post-filter
    firfilt_rrrf_destroy(_q->postfilter);

    // destroy nco object
    nco_crcf_destroy(_q->oscillator);

    // free main object memory
    free(_q);
}

// print modulation internals
void freqdem_print(freqdem _q)
{
    printf("freqdem:\n");
    printf("    mod. factor :   %8.4f\n", _q->kf);
}

// reset modem object
void freqdem_reset(freqdem _q)
{
    // reset oscillator, phase-locked loop
    nco_crcf_reset(_q->oscillator);

    // clear complex phase term
    _q->q = 0.0f;
}

// demodulate sample
//  _q      :   FM demodulator object
//  _r      :   received signal
//  _m      :   output message signal
void freqdem_demodulate(freqdem              _q,
                        liquid_float_complex _r,
                        float *              _m)
{
    if (_q->type == LIQUID_FREQDEM_PLL) {
        // 
        // push through phase-locked loop
        //

        // compute phase error from internal NCO complex exponential
        float complex p;
        nco_crcf_cexpf(_q->oscillator, &p);
        float phase_error = cargf( conjf(p)*_r );

        // step the PLL and the internal NCO object
        nco_crcf_pll_step(_q->oscillator, phase_error);
        nco_crcf_step(_q->oscillator);

        // demodulated signal is (weighted) nco frequency
        *_m = (nco_crcf_get_frequency(_q->oscillator) -_q->dphi) * _q->twopikf_inv;
    } else {
        // compute phase difference and normalize by modulation index
        *_m = (cargf(conjf(_q->q)*(_r)) - _q->dphi) * _q->twopikf_inv;

        _q->q = _r;
    }

    // apply post-filtering
    //iirfilt_rrrf_execute(_q->postfilter, *_m, _m);
}


