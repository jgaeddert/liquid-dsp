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
// firdecim.c
//
// finite impulse response decimator object definitions
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// decimator structure
struct FIRDECIM(_s) {
    TC * h;             // coefficients array
    unsigned int h_len; // number of coefficients
    unsigned int M;     // decimation factor

    WINDOW() w;         // buffer
    DOTPROD() dp;       // vector dot product
};

// create decimator object
//  _M      :   decimation factor
//  _h      :   filter coefficients [size: _h_len x 1]
//  _h_len  :   filter coefficients length
FIRDECIM() FIRDECIM(_create)(unsigned int _M,
                             TC *         _h,
                             unsigned int _h_len)
{
    // validate input
    if (_h_len == 0) {
        fprintf(stderr,"error: decim_%s_create(), filter length must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    } else if (_M == 0) {
        fprintf(stderr,"error: decim_%s_create(), decimation factor must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    }

    FIRDECIM() q = (FIRDECIM()) malloc(sizeof(struct FIRDECIM(_s)));
    q->h_len = _h_len;
    q->M     = _M;

    // allocate memory for coefficients
    q->h = (TC*) malloc((q->h_len)*sizeof(TC));

    // load filter in reverse order
    unsigned int i;
    for (i=0; i<q->h_len; i++)
        q->h[i] = _h[_h_len-i-1];

    // create window (internal buffer)
    q->w = WINDOW(_create)(q->h_len);

    // create dot product object
    q->dp = DOTPROD(_create)(q->h, q->h_len);

    // reset filter state (clear buffer)
    FIRDECIM(_clear)(q);

    return q;
}

// create decimator from prototype
//  _M      :   decimolation factor
//  _m      :   symbol delay
//  _As     :   stop-band attenuation [dB]
FIRDECIM() FIRDECIM(_create_prototype)(unsigned int _M,
                                       unsigned int _m,
                                       float        _As)
{
    // validate input
    if (_M < 2) {
        fprintf(stderr,"error: decim_%s_create_prototype(), decim factor must be greater than 1\n", EXTENSION_FULL);
        exit(1);
    } else if (_m == 0) {
        fprintf(stderr,"error: decim_%s_create_prototype(), filter delay must be greater than 0\n", EXTENSION_FULL);
        exit(1);
    } else if (_As < 0.0f) {
        fprintf(stderr,"error: decim_%s_create_prototype(), stop-band attenuation must be positive\n", EXTENSION_FULL);
        exit(1);
    }

    // compute filter coefficients (floating point precision)
    unsigned int h_len = 2*_M*_m + 1;
    float hf[h_len];
    float fc = 0.5f / (float) (_M);
    liquid_firdes_kaiser(h_len, fc, _As, 0.0f, hf);

    // copy coefficients to type-specific array (e.g. float complex)
    TC hc[h_len];
    unsigned int i;
    for (i=0; i<h_len; i++)
        hc[i] = hf[i];
    
    // return decimator object
    return FIRDECIM(_create)(_M, hc, 2*_M*_m);
}

// create square-root Nyquist decimator
//  _type   :   filter type (e.g. LIQUID_RNYQUIST_RRC)
//  _M      :   samples/symbol _M > 1
//  _m      :   filter delay (symbols), _m > 0
//  _beta   :   excess bandwidth factor, 0 < _beta < 1
//  _dt     :   fractional sample delay, 0 <= _dt < 1
FIRDECIM() FIRDECIM(_create_rnyquist)(int          _type,
                                      unsigned int _M,
                                      unsigned int _m,
                                      float        _beta,
                                      float        _dt)
{
    // validate input
    if (_M < 2) {
        fprintf(stderr,"error: decim_%s_create_rnyquist(), decimation factor must be greater than 1\n", EXTENSION_FULL);
        exit(1);
    } else if (_m == 0) {
        fprintf(stderr,"error: decim_%s_create_rnyquist(), filter delay must be greater than 0\n", EXTENSION_FULL);
        exit(1);
    } else if (_beta < 0.0f || _beta > 1.0f) {
        fprintf(stderr,"error: decim_%s_create_rnyquist(), filter excess bandwidth factor must be in [0,1]\n", EXTENSION_FULL);
        exit(1);
    } else if (_dt < -1.0f || _dt > 1.0f) {
        fprintf(stderr,"error: decim_%s_create_rnyquist(), filter fractional sample delay must be in [-1,1]\n", EXTENSION_FULL);
        exit(1);
    }

    // generate square-root Nyquist filter
    unsigned int h_len = 2*_M*_m + 1;
    float h[h_len];
    liquid_firdes_rnyquist(_type,_M,_m,_beta,_dt,h);

    // copy coefficients to type-specific array (e.g. float complex)
    unsigned int i;
    TC hc[h_len];
    for (i=0; i<h_len; i++)
        hc[i] = h[i];

    // return decimator object
    return FIRDECIM(_create)(_M, hc, h_len);
}

// destroy decimator object
void FIRDECIM(_destroy)(FIRDECIM() _q)
{
    WINDOW(_destroy)(_q->w);
    DOTPROD(_destroy)(_q->dp);
    free(_q->h);
    free(_q);
}

// print decimator object internals
void FIRDECIM(_print)(FIRDECIM() _q)
{
    printf("FIRDECIM() [%u] :\n", _q->M);
    printf("  window:\n");
    WINDOW(_print)(_q->w);
}

// clear decimator object
void FIRDECIM(_clear)(FIRDECIM() _q)
{
    WINDOW(_clear)(_q->w);
}

// execute decimator
//  _q      :   decimator object
//  _x      :   input sample array [size: _M x 1]
//  _y      :   output sample pointer
void FIRDECIM(_execute)(FIRDECIM() _q,
                        TI *       _x,
                        TO *       _y)
{
    TI * r; // read pointer
    unsigned int i;
    for (i=0; i<_q->M; i++) {
        WINDOW(_push)(_q->w, _x[i]);

        if (i==0) {
            // read buffer (retrieve pointer to aligned memory array)
            WINDOW(_read)(_q->w, &r);

            // execute dot product
            DOTPROD(_execute)(_q->dp, r, _y);
        }
    }
}

// execute decimator on block of _n*_M input samples
//  _q      : decimator object
//  _x      : input array [size: _n*_M x 1]
//  _n      : number of _output_ samples
//  _y      : output array [_sze: _n x 1]
void FIRDECIM(_execute_block)(FIRDECIM()   _q,
                              TI *         _x,
                              unsigned int _n,
                              TO *         _y)
{
    unsigned int i;
    for (i=0; i<_n; i++) {
        // execute _M input samples computing just one output each time
        FIRDECIM(_execute)(_q, &_x[i*_q->M], &_y[i]);
    }
}

