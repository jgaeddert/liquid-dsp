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
// gmskmod.c : Gauss minimum-shift keying modem
//

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "liquid.internal.h"

struct gmskmod_s {
    unsigned int k;         // samples/symbol
    unsigned int m;         // symbol delay
    float BT;               // bandwidth/time product
    unsigned int h_len;     // filter length
    float * h;              // pulse shaping filter

    // interpolator
    firinterp_rrrf interp_tx;

    float theta;            // phase state
    float k_inv;            // 1/k
};

// create gmskmod object
//  _k      :   samples/symbol
//  _m      :   filter delay (symbols)
//  _BT     :   excess bandwidth factor
gmskmod gmskmod_create(unsigned int _k,
                       unsigned int _m,
                       float _BT)
{
    if (_k < 2) {
        fprintf(stderr,"error: gmskmod_create(), samples/symbol must be at least 2\n");
        exit(1);
    } else if (_m < 1) {
        fprintf(stderr,"error: gmskmod_create(), symbol delay must be at least 1\n");
        exit(1);
    } else if (_BT <= 0.0f || _BT >= 1.0f) {
        fprintf(stderr,"error: gmskmod_create(), bandwidth/time product must be in (0,1)\n");
        exit(1);
    }

    gmskmod q = (gmskmod)malloc(sizeof(struct gmskmod_s));

    // set properties
    q->k  = _k;
    q->m  = _m;
    q->BT = _BT;

    // derived values
    q->k_inv = 1.0f / (float)(q->k);

    // allocate memory for filter taps
    q->h_len = 2*(q->k)*(q->m)+1;
    q->h = (float*) malloc(q->h_len * sizeof(float));

    // compute filter coefficients
    liquid_firdes_gmsktx(q->k, q->m, q->BT, 0.0f, q->h);

    // create interpolator object
    q->interp_tx = firinterp_rrrf_create_rnyquist(LIQUID_FIRFILT_GMSKTX, q->k, q->m, q->BT, 0);

    // reset modem state
    gmskmod_reset(q);

    // return modem object
    return q;
}

void gmskmod_destroy(gmskmod _q)
{
    // destroy interpolator
    firinterp_rrrf_destroy(_q->interp_tx);

    // free transmit filter array
    free(_q->h);

    // free main object memory
    free(_q);
}

void gmskmod_print(gmskmod _q)
{
    printf("gmskmod [k=%u, m=%u, BT=%8.3f]\n", _q->k, _q->m, _q->BT);
    unsigned int i;
    for (i=0; i<_q->h_len; i++)
        printf("  ht(%4u) = %12.8f;\n", i+1, _q->h[i]);

}

void gmskmod_reset(gmskmod _q)
{
    // reset phase state
    _q->theta = 0.0f;

    // clear interpolator buffer
    firinterp_rrrf_reset(_q->interp_tx);
}

void gmskmod_modulate(gmskmod _q,
                      unsigned int _s,
                      float complex * _y)
{
    // generate sample from symbol
    float x = _s==0 ? -_q->k_inv : _q->k_inv;

    // run interpolator
    float phi[_q->k];
    firinterp_rrrf_execute(_q->interp_tx, x, phi);

    // integrate phase state
    unsigned int i;
    for (i=0; i<_q->k; i++) {
        // integrate phase state
        _q->theta += phi[i];

        // ensure phase in [-pi, pi]
        if (_q->theta >  M_PI) _q->theta -= 2*M_PI;
        if (_q->theta < -M_PI) _q->theta += 2*M_PI;

        // compute output
        _y[i] = liquid_cexpjf(_q->theta);
    }


}


