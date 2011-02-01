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
// gmskdem.c : Gauss minimum-shift keying modem
//

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "liquid.internal.h"

#define DEBUG_GMSKDEM           1
#define DEBUG_GMSKDEM_FILENAME  "gmskdem_internal_debug.m"
#define DEBUG_BUFFER_LEN        (1000)

void gmskdem_debug_print(gmskdem _q,
                         const char * _filename);

struct gmskdem_s {
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
    float mu;           // equalizer update rate
    int enable_equalizer;   // update equalizer flag

    // demodulated symbols counter
    unsigned int num_symbols_demod;

#if DEBUG_GMSKDEM
    windowf  debug_eqout;
#endif
};

// create gmskdem object
//  _k      :   samples/symbol
//  _m      :   filter delay (symbols)
//  _BT     :   excess bandwidth factor
gmskdem gmskdem_create(unsigned int _k,
                       unsigned int _m,
                       float _BT)
{
    if (_k < 2) {
        fprintf(stderr,"error: gmskdem_create(), samples/symbol must be at least 2\n");
        exit(1);
    } else if (_m < 1) {
        fprintf(stderr,"error: gmskdem_create(), symbol delay must be at least 1\n");
        exit(1);
    } else if (_BT <= 0.0f || _BT >= 1.0f) {
        fprintf(stderr,"error: gmskdem_create(), bandwidth/time product must be in (0,1)\n");
        exit(1);
    }

    gmskdem q = (gmskdem)malloc(sizeof(struct gmskdem_s));

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

    // create equalizer
    float htmp[q->h_len];
#if 0
    for (i=0; i<q->h_len; i++)
        htmp[i] = ( i==(q->k)*(q->m) ) ? 1.0 : 0.0;
#else
    for (i=0; i<q->h_len; i++) {
        t = (float)i/(float)(q->k)-(float)(q->m);
        float sig = 0.02f;
        htmp[i] = expf(-t*t/(2*sig*sig));
    }
#endif
    q->eq = eqlms_rrrf_create(htmp, q->h_len);
    gmskdem_set_bw(q, 0.2f);
    gmskdem_enable_eq(q);

    // reset modem state
    gmskdem_reset(q);

#if DEBUG_GMSKDEM
    q->debug_eqout  = windowf_create(DEBUG_BUFFER_LEN);
#endif

    // return modem object
    return q;
}

void gmskdem_destroy(gmskdem _q)
{
#if DEBUG_GMSKDEM
    // print to external file
    gmskdem_debug_print(_q, DEBUG_GMSKDEM_FILENAME);

    // destroy debugging objects
    windowf_destroy(_q->debug_eqout);
#endif

    // destroy filter object
    firfilt_rrrf_destroy(_q->filter);

    // destroy equalizer object
    //eqlms_rrrf_print(_q->eq);
    eqlms_rrrf_destroy(_q->eq);

    free(_q->h);
    free(_q);
}

void gmskdem_print(gmskdem _q)
{
    printf("gmskdem:\n");
    unsigned int i;
    for (i=0; i<_q->h_len; i++)
        printf("  h(%4u) = %12.8f;\n", i+1, _q->h[i]);

}

void gmskdem_reset(gmskdem _q)
{
    // reset phase state
    _q->theta = 0.0f;

    _q->x_prime = 0.0f;

    // set demod. counter to zero
    _q->num_symbols_demod = 0;

    // clear filter buffer
    firfilt_rrrf_clear(_q->filter);

    // reset equalizer
    eqlms_rrrf_reset(_q->eq);
}

// set the bandwidth of the equalizer
void gmskdem_set_bw(gmskdem _q, float _mu)
{
    // validate input
    if (_mu < 0.0f) {
        fprintf(stderr,"error: gmskdem_set_bw(), equalizer bandwidth must be greater than zero\n");
        exit(1);
    }

    _q->mu = _mu;
    eqlms_rrrf_set_bw(_q->eq, _q->mu);
}

// enable updating equalizer
void gmskdem_enable_eq(gmskdem _q)
{
    _q->enable_equalizer = 1;
}

// disable updating equalizer
void gmskdem_disable_eq(gmskdem _q)
{
    _q->enable_equalizer = 0;
}

void gmskdem_demodulate(gmskdem _q,
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
#if DEBUG_GMSKDEM
        // compute output
        float d_tmp;
        eqlms_rrrf_execute(_q->eq, &d_tmp);
        windowf_push(_q->debug_eqout, d_tmp);
#endif

        // compute filter output (decimate)
        if (i == _q->k-1) {
            // compute filter output
            eqlms_rrrf_execute(_q->eq, &d_hat);
            //printf("d_hat : %12.8f\n", d_hat);

            // train equalizer, but wait until internal
            // buffer is full
            if (_q->enable_equalizer &&
                _q->num_symbols_demod >= _q->m)
            {
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

//
// output debugging file
//
void gmskdem_debug_print(gmskdem _q,
                         const char * _filename)
{
    // open output filen for writing
    FILE * fid = fopen(_filename,"w");
    if (!fid) {
        fprintf(stderr,"error: gmskdem_debug_print(), could not open '%s' for writing\n", _filename);
        exit(1);
    }
    fprintf(fid,"%% %s : auto-generated file\n", _filename);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");

#if DEBUG_GMSKDEM
    //
    unsigned int i;
    float * r;
    fprintf(fid,"n = %u;\n", DEBUG_BUFFER_LEN);
    fprintf(fid,"k = %u;\n", _q->k);
    fprintf(fid,"m = %u;\n", _q->m);
    fprintf(fid,"t = [0:(n-1)]/k;\n");

    fprintf(fid,"eqout = zeros(1,n);\n");
    windowf_read(_q->debug_eqout, &r);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        fprintf(fid,"eqout(%5u) = %12.4e;\n", i+1, r[i]);
    fprintf(fid,"i0 = mod(k+n,k)+k;\n");
    fprintf(fid,"isym = i0:k:n;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,eqout,'-', t(isym),eqout(isym),'o','MarkerSize',1);\n");
#endif

    fclose(fid);
    printf("gmskdem: internal debugging written to '%s'\n", _filename);
}
