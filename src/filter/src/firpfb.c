/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

//
// FIR Polyphase filter bank
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct FIRPFB(_s) {
    TC * h;                     // filter coefficients array
    unsigned int h_len;         // total number of filter coefficients
    unsigned int h_sub_len;     // sub-sampled filter length
    unsigned int num_filters;   // number of filters

    WINDOW() w;                 // window buffer
    DOTPROD() * dp;             // array of vector dot product objects
    TC scale;                   // output scaling factor
};

// create firpfb from external coefficients
//  _M      : number of filters in the bank
//  _h      : coefficients [size: _M*_h_len x 1]
//  _h_len  : filter delay (symbols)
FIRPFB() FIRPFB(_create)(unsigned int _M,
                         TC *         _h,
                         unsigned int _h_len)
{
    // validate input
    if (_M == 0) {
        fprintf(stderr,"error: firpfb_%s_create(), number of filters must be greater than zero\n",
                EXTENSION_FULL);
        exit(1);
    } else if (_h_len == 0) {
        fprintf(stderr,"error: firpfb_%s_create(), filter length must be greater than zero\n",
                EXTENSION_FULL);
        exit(1);
    }

    // create main filter object
    FIRPFB() q = (FIRPFB()) malloc(sizeof(struct FIRPFB(_s)));

    // set user-defined parameters
    q->num_filters = _M;
    q->h_len       = _h_len;

    // each filter is realized as a dotprod object
    q->dp = (DOTPROD()*) malloc((q->num_filters)*sizeof(DOTPROD()));

    // generate bank of sub-samped filters
    // length of each sub-sampled filter
    unsigned int h_sub_len = _h_len / q->num_filters;
    TC h_sub[h_sub_len];
    unsigned int i, n;
    for (i=0; i<q->num_filters; i++) {
        for (n=0; n<h_sub_len; n++) {
            // load filter in reverse order
            h_sub[h_sub_len-n-1] = _h[i + n*(q->num_filters)];
        }

        // create dot product object
        q->dp[i] = DOTPROD(_create)(h_sub,h_sub_len);
    }

    // save sub-sampled filter length
    q->h_sub_len = h_sub_len;

    // create window buffer
    q->w = WINDOW(_create)(q->h_sub_len);

    // set default scaling
    q->scale = 1;

    // reset object and return
    FIRPFB(_reset)(q);
    return q;
}

// create firpfb from external coefficients
//  _M      : number of filters in the bank
//  _m      : filter semi-length [samples]
//  _fc     : filter cut-off frequency 0 < _fc < 0.5
//  _As     : filter stop-band suppression [dB]
FIRPFB() FIRPFB(_create_kaiser)(unsigned int _M,
                                unsigned int _m,
                                float        _fc,
                                float        _As)
{
    // validate input
    if (_M == 0) {
        fprintf(stderr,"error: firpfb_%s_create_kaiser(), number of filters must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    } else if (_m == 0) {
        fprintf(stderr,"error: firpfb_%s_create_kaiser(), filter delay must be greater than 0\n", EXTENSION_FULL);
        exit(1);
    } else if (_fc < 0.0f || _fc > 0.5f) {
        fprintf(stderr,"error: firpfb_%s_create_kaiser(), filter cut-off frequence must be in (0,0.5)\n", EXTENSION_FULL);
        exit(1);
    } else if (_As < 0.0f) {
        fprintf(stderr,"error: firpfb_%s_create_kaiser(), filter excess bandwidth factor must be in [0,1]\n", EXTENSION_FULL);
        exit(1);
    }

    // generate square-root Nyquist filter
    unsigned int H_len = 2*_M*_m + 1;
    float Hf[H_len];
    liquid_firdes_kaiser(H_len, _fc/(float)_M, _As, 0.0f, Hf);

    // copy coefficients to type-specific array (e.g. float complex)
    unsigned int i;
    TC Hc[H_len];
    for (i=0; i<H_len; i++)
        Hc[i] = Hf[i];

    // return filterbank object
    return FIRPFB(_create)(_M, Hc, H_len);
}

// create square-root Nyquist filterbank
//  _type   :   filter type (e.g. LIQUID_RNYQUIST_RRC)
//  _M      :   number of filters in the bank
//  _k      :   samples/symbol _k > 1
//  _m      :   filter delay (symbols), _m > 0
//  _beta   :   excess bandwidth factor, 0 < _beta < 1
FIRPFB() FIRPFB(_create_rnyquist)(int          _type,
                                  unsigned int _M,
                                  unsigned int _k,
                                  unsigned int _m,
                                  float        _beta)
{
    // validate input
    if (_M == 0) {
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
    unsigned int H_len = 2*_M*_k*_m + 1;
    float Hf[H_len];
    liquid_firdes_prototype(_type,_M*_k,_m,_beta,0,Hf);

    // copy coefficients to type-specific array (e.g. float complex)
    unsigned int i;
    TC Hc[H_len];
    for (i=0; i<H_len; i++)
        Hc[i] = Hf[i];

    // return filterbank object
    return FIRPFB(_create)(_M, Hc, H_len);
}

// create firpfb derivative square-root Nyquist filterbank
//  _type   :   filter type (e.g. LIQUID_RNYQUIST_RRC)
//  _M      :   number of filters in the bank
//  _k      :   samples/symbol _k > 1
//  _m      :   filter delay (symbols), _m > 0
//  _beta   :   excess bandwidth factor, 0 < _beta < 1
FIRPFB() FIRPFB(_create_drnyquist)(int          _type,
                                   unsigned int _M,
                                   unsigned int _k,
                                   unsigned int _m,
                                   float        _beta)
{
    // validate input
    if (_M == 0) {
        fprintf(stderr,"error: firpfb_%s_create_drnyquist(), number of filters must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    } else if (_k < 2) {
        fprintf(stderr,"error: firpfb_%s_create_drnyquist(), filter samples/symbol must be greater than 1\n", EXTENSION_FULL);
        exit(1);
    } else if (_m == 0) {
        fprintf(stderr,"error: firpfb_%s_create_drnyquist(), filter delay must be greater than 0\n", EXTENSION_FULL);
        exit(1);
    } else if (_beta < 0.0f || _beta > 1.0f) {
        fprintf(stderr,"error: firpfb_%s_create_drnyquist(), filter excess bandwidth factor must be in [0,1]\n", EXTENSION_FULL);
        exit(1);
    }

    // generate square-root Nyquist filter
    unsigned int H_len = 2*_M*_k*_m + 1;
    float Hf[H_len];
    liquid_firdes_prototype(_type,_M*_k,_m,_beta,0,Hf);
    
    // compute derivative filter
    float dHf[H_len];
    float HdH_max = 0.0f;
    unsigned int i;
    for (i=0; i<H_len; i++) {
        if (i==0) {
            dHf[i] = Hf[i+1] - Hf[H_len-1];
        } else if (i==H_len-1) {
            dHf[i] = Hf[0]   - Hf[i-1];
        } else {
            dHf[i] = Hf[i+1] - Hf[i-1];
        }

        // find maximum of h*dh
        if ( fabsf(Hf[i]*dHf[i]) > HdH_max )
            HdH_max = fabsf(Hf[i]*dHf[i]);
    }

    // copy coefficients to type-specific array (e.g. float complex)
    // and apply scaling factor for normalized response
    TC Hc[H_len];
    for (i=0; i<H_len; i++)
        Hc[i] = dHf[i] * 0.06f / HdH_max;

    // return filterbank object
    return FIRPFB(_create)(_M, Hc, H_len);
}


// re-create filterbank object
//  _q      : original firpfb object
//  _M      : number of filters in the bank
//  _h      : coefficients [size: _M x _h_len]
//  _h_len  : length of each filter
FIRPFB() FIRPFB(_recreate)(FIRPFB()     _q,
                           unsigned int _M,
                           TC *         _h,
                           unsigned int _h_len)
{
    // check to see if filter length has changed
    if (_h_len != _q->h_len || _M != _q->num_filters) {
        // filter length has changed: recreate entire filter
        FIRPFB(_destroy)(_q);
        _q = FIRPFB(_create)(_M,_h,_h_len);
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

// destroy firpfb object, freeing all internal memory
void FIRPFB(_destroy)(FIRPFB() _q)
{
    unsigned int i;
    for (i=0; i<_q->num_filters; i++)
        DOTPROD(_destroy)(_q->dp[i]);
    free(_q->dp);
    WINDOW(_destroy)(_q->w);
    free(_q);
}

// print firpfb object's parameters
void FIRPFB(_print)(FIRPFB() _q)
{
    printf("fir polyphase filterbank [%u] :\n", _q->num_filters);
    unsigned int i,n;

    for (i=0; i<_q->num_filters; i++) {
        printf("  bank %3u: ",i);
        for (n=0; n<_q->h_len; n++) {
            //printf("%6.4f+j%6.4f ", crealf(_q->dp[i]->h[n]), cimagf(_q->dp[i]->h[n]));
        }
        printf("\n");
    }
}

// clear/reset firpfb object internal state
void FIRPFB(_reset)(FIRPFB() _q)
{
    WINDOW(_clear)(_q->w);
}

// set output scaling for filter
void FIRPFB(_set_scale)(FIRPFB() _q,
                         TC      _scale)
{
    _q->scale = _scale;
}

// push sample into firpfb internal buffer
void FIRPFB(_push)(FIRPFB() _q, TI _x)
{
    // push value into window buffer
    WINDOW(_push)(_q->w, _x);
}

// execute the filter on internal buffer and coefficients
//  _q      : firpfb object
//  _i      : index of filter to use
//  _y      : pointer to output sample
void FIRPFB(_execute)(FIRPFB()     _q,
                      unsigned int _i,
                      TO *         _y)
{
    // validate input
    if (_i >= _q->num_filters) {
        fprintf(stderr,"error: firpfb_execute(), filterbank index (%u) exceeds maximum (%u)\n",
                _i, _q->num_filters);
        exit(1);
    }

    // read buffer
    TI *r;
    WINDOW(_read)(_q->w, &r);

    // execute dot product
    DOTPROD(_execute)(_q->dp[_i], r, _y);

    // apply scaling factor
    *_y *= _q->scale;
}

