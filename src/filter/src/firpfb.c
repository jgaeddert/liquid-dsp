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
// FIR Polyphase filter bank
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// defined:
//  FIRPFB()    name-mangling macro
//  TO          output data type
//  TC          coefficients data type
//  TI          input data type
//  WINDOW()    window macro
//  DOTPROD()   dotprod macro
//  PRINTVAL()  print macro

struct FIRPFB(_s) {
    TC * h;                     // filter coefficients array
    unsigned int h_len;         // total number of filter coefficients
    unsigned int h_sub_len;     // sub-sampled filter length
    unsigned int num_filters;   // number of filters

    WINDOW() w;
    DOTPROD() * dp;

#if 0
    fir_prototype p;    // prototype object
#endif
};

FIRPFB() FIRPFB(_create)(unsigned int _num_filters, TC * _h, unsigned int _h_len)
{
    FIRPFB() b = (FIRPFB()) malloc(sizeof(struct FIRPFB(_s)));
    b->num_filters = _num_filters;
    b->h_len = _h_len;

    // each filter is realized as a dotprod object
    b->dp = (DOTPROD()*) malloc((b->num_filters)*sizeof(DOTPROD()));

    // generate bank of sub-samped filters
    // length of each sub-sampled filter
    unsigned int h_sub_len = _h_len / b->num_filters;
    TC h_sub[h_sub_len];
    unsigned int i, n;
    for (i=0; i<b->num_filters; i++) {
        for (n=0; n<h_sub_len; n++) {
            // load filter in reverse order
            h_sub[h_sub_len-n-1] = _h[i + n*(b->num_filters)];
        }

        b->dp[i] = DOTPROD(_create)(h_sub,h_sub_len);
    }

    // save sub-sampled filter length
    b->h_sub_len = h_sub_len;

    // create window buffer
    b->w = WINDOW(_create)(b->h_sub_len);
    WINDOW(_clear)(b->w);

    return b;
}

// create square-root Nyquist filterbank
//  _type   :   filter type (e.g. LIQUID_RNYQUIST_RRC)
//  _npfb   :   number of filters in the bank
//  _k      :   samples/symbol _k > 1
//  _m      :   filter delay (symbols), _m > 0
//  _beta   :   excess bandwidth factor, 0 < _beta < 1
FIRPFB() FIRPFB(_create_rnyquist)(int _type,
                                  unsigned int _npfb,
                                  unsigned int _k,
                                  unsigned int _m,
                                  float _beta)
{
    // validate input
    if (_npfb == 0) {
        fprintf(stderr,"error: firpfb_%s_create_rnyquist(), number of filters must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    } else if (_k < 2) {
        fprintf(stderr,"error: firpfb_%s_create_rnyquist(), filter samples/symbol must be greater than 1\n", EXTENSION_FULL);
        exit(1);
    } else if (_m == 0) {
        fprintf(stderr,"error: firpfb_%s_create_rnyquist(), filter delay must be greater than 0\n", EXTENSION_FULL);
        exit(1);
    } else if (_beta < 0.0f || _beta > 1.0f) {
        fprintf(stderr,"error: firpfb_%s_create_rnyquist(), filter excess bandwidth factor must be in [0,1]\n", EXTENSION_FULL);
        exit(1);
    }

    // generate square-root Nyquist filter
    unsigned int H_len = 2*_npfb*_k*_m + 1;
    float Hf[H_len];
    liquid_firdes_rnyquist(_type,_npfb*_k,_m,_beta,0,Hf);

    // copy coefficients to type-specific array (e.g. float complex)
    unsigned int i;
    TC Hc[H_len];
    for (i=0; i<H_len; i++)
        Hc[i] = Hf[i];

    // return filterbank object
    return FIRPFB(_create)(_npfb, Hc, H_len);
}


// re-create filterbank object
// TODO : check this method
FIRPFB() FIRPFB(_recreate)(FIRPFB() _q,
                           unsigned int _num_filters,
                           TC * _h,
                           unsigned int _h_len)
{
    // check to see if filter length has changed
    if (_h_len != _q->h_len || _num_filters != _q->num_filters) {
        // filter length has changed: recreate entire filter
        FIRPFB(_destroy)(_q);
        _q = FIRPFB(_create)(_num_filters,_h,_h_len);
        return _q;
    }

    // re-create each dotprod object
    TC h_sub[_q->h_sub_len];
    unsigned int i, n;
    for (i=0; i<_q->num_filters; i++) {
        for (n=0; n<_q->h_sub_len; n++) {
            // load filter in reverse order
            h_sub[_q->h_sub_len-n-1] = _h[i + n*(_q->num_filters)];
        }

        _q->dp[i] = DOTPROD(_recreate)(_q->dp[i],h_sub,_q->h_sub_len);
    }
    return _q;
}

void FIRPFB(_destroy)(FIRPFB() _b)
{
    unsigned int i;
    for (i=0; i<_b->num_filters; i++)
        DOTPROD(_destroy)(_b->dp[i]);
    free(_b->dp);
    WINDOW(_destroy)(_b->w);
    free(_b);
}

void FIRPFB(_print)(FIRPFB() _b)
{
    printf("fir polyphase filterbank [%u] :\n", _b->num_filters);
    unsigned int i,n;

    for (i=0; i<_b->num_filters; i++) {
        printf("  bank %3u: ",i);
        for (n=0; n<_b->h_len; n++) {
            //printf("%6.4f+j%6.4f ", crealf(_b->dp[i]->h[n]), cimagf(_b->dp[i]->h[n]));
        }
        printf("\n");
    }
}

void FIRPFB(_push)(FIRPFB() _b, TI _x)
{
    // push value into window buffer
    WINDOW(_push)(_b->w, _x);
}

void FIRPFB(_execute)(FIRPFB() _b,
                      unsigned int _i,
                      TO *_y)
{
    // validate input
    if (_i >= _b->num_filters) {
        fprintf(stderr,"error: firpfb_execute(), filterbank index exceeds maximum\n");
        exit(1);
    }

    // read buffer
    TI *r;
    WINDOW(_read)(_b->w, &r);

    // execute dot product
    DOTPROD(_execute)(_b->dp[_i], r, _y);
}

void FIRPFB(_clear)(FIRPFB() _b)
{
    WINDOW(_clear)(_b->w);
}

