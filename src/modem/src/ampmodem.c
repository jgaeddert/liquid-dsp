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
// Amplitude modulator/demodulator
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "liquid.internal.h"

struct ampmodem_s {
    float m;
    liquid_modem_amtype type;

    // demod objects
    nco oscillator;
    pll sync;

    // ssb
    // TODO : replace DC bias removal with iir filter object
    float ssb_alpha;    // dc bias removal
    float ssb_q_hat;

    // dsb
};

ampmodem ampmodem_create(float _m,
                         liquid_modem_amtype _type)
{
    ampmodem q = (ampmodem) malloc(sizeof(struct ampmodem_s));
    q->type = _type;
    q->m    = _m;

    // create nco, pll objects
    q->oscillator = nco_create(LIQUID_NCO);
    nco_reset(q->oscillator);
    q->sync = pll_create();
    pll_reset(q->sync);

    // single side-band
    q->ssb_alpha = 1e-3f;
    q->ssb_q_hat = 0.0f;

    // double side-band

    ampmodem_reset(q);

    return q;
}

void ampmodem_destroy(ampmodem _q)
{
    nco_destroy(_q->oscillator);
    pll_destroy(_q->sync);
    free(_q);
}

void ampmodem_print(ampmodem _q)
{
    printf("ampmodem [%s]:\n", _q->type == LIQUID_MODEM_AM_SSB ? "ssb" : "dsb");
    printf("    mod. index  :   %8.4f\n", _q->m);
}

void ampmodem_reset(ampmodem _q)
{
}

void ampmodem_modulate(ampmodem _q,
                       float _x,
                       float complex *_y)
{
    switch (_q->type) {
    case LIQUID_MODEM_AM_SSB:
        *_y = _x + 0.5f;
        break;
    case LIQUID_MODEM_AM_DSB:
        *_y = _x;
        break;
    default:
        fprintf(stderr,"error: ampmodem_modulate(), invalid type\n");
        exit(1);
    }
}

void ampmodem_demodulate(ampmodem _q,
                         float complex _y,
                         float *_x)
{
    switch (_q->type) {
    case LIQUID_MODEM_AM_SSB:
        // peak detector
#if 0
        float q = cabsf(_y);

        // remove DC bias
        _q->ssb_q_hat = (_q->ssb_alpha)*q + (1.0f - _q->ssb_alpha)*_q->ssb_q_hat;
        *_y = q - _q->ssb_q_hat;
#endif
        break;
    case LIQUID_MODEM_AM_DSB:
        break;
    default:
        fprintf(stderr,"error: ampmodem_demodulate(), invalid type\n");
        exit(1);
    }
}

