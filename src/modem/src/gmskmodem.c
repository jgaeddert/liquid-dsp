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
// gmskmodem.c : Gauss minimum-shift keying modem
//

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "liquid.internal.h"

struct gmskmodem_s {
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

    // demodulator
    eqlms_rrrf eq;      // equalizer

    // demodulated symbols counter
    unsigned int num_symbols_demod;
};

gmskmodem gmskmodem_create(unsigned int _k,
                           unsigned int _m,
                           float _BT)
{
    if (_k < 2) {
        fprintf(stderr,"error: gmskmodem_create(), samples/symbol must be at least 2\n");
        exit(1);
    } else if (_m < 1) {
        fprintf(stderr,"error: gmskmodem_create(), symbol delay must be at least 1\n");
        exit(1);
    } else if (_BT <= 0.0f || _BT >= 1.0f) {
        fprintf(stderr,"error: gmskmodem_create(), bandwidth/time product must be in (0,1)\n");
        exit(1);
    }

    gmskmodem q = (gmskmodem)malloc(sizeof(struct gmskmodem_s));

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

    // demodulator, create equalizer
    float htmp[q->h_len];
    for (i=0; i<q->h_len; i++)
        htmp[i] = ( i==(q->k)*(q->m) ) ? 1.0 : 0.0;
    q->eq = eqlms_rrrf_create(htmp, q->h_len);
    eqlms_rrrf_set_bw(q->eq, 0.05f);

    // reset modem state
    gmskmodem_reset(q);

    // return modem object
    return q;
}

void gmskmodem_destroy(gmskmodem _q)
{
    // destroy filter object
    firfilt_rrrf_destroy(_q->filter);

    // destroy equalizer object
    //eqlms_rrrf_print(_q->eq);
    eqlms_rrrf_destroy(_q->eq);

    // set demod. counter to zero
    _q->num_symbols_demod = 0;

    free(_q->h);
    free(_q);
}

void gmskmodem_print(gmskmodem _q)
{
    printf("gmskmodem:\n");
    unsigned int i;
    for (i=0; i<_q->h_len; i++)
        printf("  h(%4u) = %12.8f;\n", i+1, _q->h[i]);

}

void gmskmodem_reset(gmskmodem _q)
{
    // reset phase state
    _q->theta = 0.0f;

    _q->x_prime = 0.0f;

    // clear filter buffer
    firfilt_rrrf_clear(_q->filter);

    // reset equalizer
    eqlms_rrrf_reset(_q->eq);
}

void gmskmodem_modulate(gmskmodem _q,
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

void gmskmodem_demodulate(gmskmodem _q,
                          float complex * _x,
                          unsigned int * _s)
{
    // run filter as interpolator
    unsigned int i;
    float phi;
    float d_hat;
    for (i=0; i<_q->k; i++) {
        // compute phase difference
        phi = cargf( conjf(_q->x_prime)*_x[i] );
        _q->x_prime = _x[i];

        // run through equalizer
        eqlms_rrrf_push(_q->eq, phi);

        // compute filter output (decimate)
        if (i == _q->k-1) {
            // compute filter output
            eqlms_rrrf_execute(_q->eq, &d_hat);
            //printf("d_hat : %12.8f\n", d_hat);

            // train equalizer, but wait until internal
            // buffer is full
            if (_q->num_symbols_demod >= _q->m) {
                // decision
                float d = d_hat > 0 ? _q->g : -_q->g;
                
                // train equalizer
                eqlms_rrrf_step(_q->eq, d, d_hat);
            }
            // increment symbol counter
            _q->num_symbols_demod++;
        }
    }

    // make decision
    *_s = d_hat > 0.0f ? 1 : 0;
}

