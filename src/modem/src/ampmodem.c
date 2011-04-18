/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Virginia Polytechnic
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
// Amplitude modulator/demodulator
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "liquid.internal.h"

struct ampmodem_s {
    float m;                    // modulation index
    liquid_modem_amtype type;   // modulation type
    int suppressed_carrier;     // suppressed carrier flag

    // demod objects
    nco_crcf oscillator;

    // suppressed carrier
    // TODO : replace DC bias removal with iir filter object
    float ssb_alpha;    // dc bias removal
    float ssb_q_hat;

    // single side-band
    firhilbf hilbert;   // hilbert transform

    // double side-band
};

// create ampmodem object
//  _m                  :   modulation index
//  _type               :   AM type (e.g. LIQUID_MODEM_AM_DSB)
//  _suppressed_carrier :   carrier suppression flag
ampmodem ampmodem_create(float _m,
                         liquid_modem_amtype _type,
                         int _suppressed_carrier)
{
    ampmodem q = (ampmodem) malloc(sizeof(struct ampmodem_s));
    q->type = _type;
    q->m    = _m;
    q->suppressed_carrier = (_suppressed_carrier == 0) ? 0 : 1;

    // create nco, pll objects
    q->oscillator = nco_crcf_create(LIQUID_NCO);
    nco_crcf_reset(q->oscillator);
    
    nco_crcf_pll_set_bandwidth(q->oscillator,1e-1f);

    // suppressed carrier
    q->ssb_alpha = 0.01f;
    q->ssb_q_hat = 0.0f;

    // single side-band
    q->hilbert = firhilbf_create(9, 60.0f);

    // double side-band

    ampmodem_reset(q);

    return q;
}

void ampmodem_destroy(ampmodem _q)
{
    nco_crcf_destroy(_q->oscillator);

    firhilbf_destroy(_q->hilbert);

    free(_q);
}

void ampmodem_print(ampmodem _q)
{
    printf("ampmodem:\n");
    printf("    type            :   ");
    switch (_q->type) {
    case LIQUID_MODEM_AM_DSB: printf("double side-band\n");         break;
    case LIQUID_MODEM_AM_USB: printf("single side-band (upper)\n"); break;
    case LIQUID_MODEM_AM_LSB: printf("single side-band (lower)\n"); break;
    default:                  printf("unknown\n");
    }
    printf("    supp. carrier   :   %s\n", _q->suppressed_carrier ? "yes" : "no");
    printf("    mod. index      :   %-8.4f\n", _q->m);
}

void ampmodem_reset(ampmodem _q)
{
    // single side-band
    _q->ssb_q_hat = 0.5f;
}

void ampmodem_modulate(ampmodem _q,
                       float _x,
                       float complex *_y)
{
    float complex x_hat = 0.0f;

    if (_q->type == LIQUID_MODEM_AM_DSB) {
        x_hat = _x;
    } else {
        // push through Hilbert transform
        // LIQUID_MODEM_AM_USB:
        // LIQUID_MODEM_AM_LSB: conjugate Hilbert transform output
        firhilbf_r2c_execute(_q->hilbert, _x, &x_hat);

        if (_q->type == LIQUID_MODEM_AM_LSB)
            x_hat = conjf(x_hat);
    }

    if (_q->suppressed_carrier)
        *_y = x_hat;
    else
        *_y = 0.5f*(x_hat + 1.0f);
}

void ampmodem_demodulate(ampmodem _q,
                         float complex _y,
                         float *_x)
{
    float t;
    float complex s;

#if 0
    switch (_q->type) {
    case LIQUID_MODEM_AM_DSB:
    case LIQUID_MODEM_AM_USB:
    case LIQUID_MODEM_AM_LSB:
    default:
        fprintf(stderr,"error: ampmodem_demodulate(), invalid type\n");
        exit(1);
    }
#endif

    if (_q->suppressed_carrier) {
        // coherent demodulation

        // mix signal down
        nco_crcf_cexpf(_q->oscillator, &s);
        _y *= s;

        // compute constrained phase error
        t = -cargf(_y);
        if      (t >  M_PI/2) t -= M_PI;
        else if (t < -M_PI/2) t += M_PI;
        t *= cabsf(_y);

        // adjust nco, pll objects
        nco_crcf_pll_step(_q->oscillator, t);
        nco_crcf_step(_q->oscillator);
        *_x = crealf(_y);
    } else {
        // non-coherent demodulation (peak detector)
        t = cabsf(_y);

        // remove DC bias
        _q->ssb_q_hat = (    _q->ssb_alpha)*t +
                        (1 - _q->ssb_alpha)*_q->ssb_q_hat;
        *_x = 2.0f*(t - _q->ssb_q_hat);
    }
}

