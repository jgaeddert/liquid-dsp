/*
 * Copyright (c) 2010, 2011 Joseph Gaeddert
 * Copyright (c) 2010, 2011 Virginia Polytechnic Institute & State University
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
    unsigned int k;     // samples/symbol
    unsigned int m;     // symbol delay
    float BT;           // bandwidth/time product
    unsigned int h_len; // filter length
    float * h;          // pulse shaping filter

    // filter object
    firfilt_rrrf filter;// transmit filter pulse shape
    float g;            // matched-filter scaling factor

    float theta;        // phase state
    float complex x_prime;
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

    // allocate memory for filter taps
    q->h_len = 2*(q->k)*(q->m)+1;
    q->h = (float*) malloc(q->h_len * sizeof(float));

    // compute filter coefficients
    // TODO : compute this properly
    unsigned int i;
#if 0
    design_rrc_filter(q->k, q->m, 0.99f, 0.0f, q->h);
#else
    float t;
    float c0 = 1.0f / sqrtf(logf(2.0f));
    for (i=0; i<q->h_len; i++) {
        t = (float)i/(float)(q->k)-(float)(q->m);

        q->h[i] = liquid_Qf(2*M_PI*q->BT*(t-0.5f)*c0) -
                  liquid_Qf(2*M_PI*q->BT*(t+0.5f)*c0);
    }
#endif

    // normalize filter coefficients such that the filter's
    // integral is pi/2
    float e = 0.0f;
    for (i=0; i<q->h_len; i++)
        e += q->h[i];
    for (i=0; i<q->h_len; i++)
        q->h[i] *= M_PI / (2.0f * e);

    // compute scaling factor
    q->g = q->h[(q->k)*(q->m)];

    // create filter object
    q->filter = firfilt_rrrf_create(q->h, q->h_len);

    // reset modem state
    gmskmod_reset(q);

    // return modem object
    return q;
}

void gmskmod_destroy(gmskmod _q)
{
    // destroy filter object
    firfilt_rrrf_destroy(_q->filter);

    // set demod. counter to zero
    free(_q->h);
    free(_q);
}

void gmskmod_print(gmskmod _q)
{
    printf("gmskmod:\n");
    unsigned int i;
    for (i=0; i<_q->h_len; i++)
        printf("  h(%4u) = %12.8f;\n", i+1, _q->h[i]);

}

void gmskmod_reset(gmskmod _q)
{
    // reset phase state
    _q->theta = 0.0f;

    _q->x_prime = 0.0f;

    // clear filter buffer
    firfilt_rrrf_clear(_q->filter);
}

void gmskmod_modulate(gmskmod _q,
                      unsigned int _s,
                      float complex * _y)
{
    // generate sample from symbol
    float x = _s==0 ? -1.0f : 1.0f;

    // run filter as interpolator
    unsigned int i;
    float phi;          // filtered phase
    for (i=0; i<_q->k; i++) {
        if (i==0)
            firfilt_rrrf_push(_q->filter, x);
        else
            firfilt_rrrf_push(_q->filter, 0.0f);

        // compute filter output
        firfilt_rrrf_execute(_q->filter, &phi);

        // integrate phase state
        _q->theta += phi;

        // compute output
        _y[i] = liquid_cexpjf(_q->theta);
    }


}


