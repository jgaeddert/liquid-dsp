/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011, 2012, 2013 Joseph Gaeddert
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
// firpfbch2.c
//
// finite impulse response polyphase filterbank channelizer with output
// rate 2 Fs / M
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// firpfbch2 object structure definition
struct FIRPFBCH2(_s) {
    int type;                   // synthesis/analysis
    unsigned int M;             // number of channels
    unsigned int M2;            // number of channels/2
    unsigned int m;             // filter semi-length

    // filter
    unsigned int h_len;         // filter length
    TC * h;                     // filter coefficients
    
    // create separate bank of dotprod and window objects
    DOTPROD() * dp;             // dot product object array
    WINDOW() * w;               // window buffer object array

    // inverse FFT plan
    FFT_PLAN ifft;              // inverse FFT object
    TO * X;                     // IFFT input array  [size: M x 1]
    TO * x;                     // IFFT output array [size: M x 1]
};

// create firpfbch2 object
//  _type   :   channelizer type (e.g. LIQUID_ANALYZER)
//  _M      :   number of channels (must be even)
//  _m      :   prototype filter semi-lenth, length=2*M*m
//  _h      :   prototype filter coefficient array
//  _h_len  :   number of coefficients
FIRPFBCH2() FIRPFBCH2(_create)(int          _type,
                               unsigned int _M,
                               unsigned int _m,
                               TC *         _h)
{
    // validate input
    if (_type != LIQUID_ANALYZER && _type != LIQUID_SYNTHESIZER) {
        fprintf(stderr,"error: firpfbch2_%s_create(), invalid type %d\n", EXTENSION_FULL, _type);
        exit(1);
    } else if (_M < 2 || _M % 2) {
        fprintf(stderr,"error: firpfbch2_%s_create(), number of channels must be greater than 2 and even\n", EXTENSION_FULL);
        exit(1);
    } else if (_m < 1) {
        fprintf(stderr,"error: firpfbch2_%s_create(), filter semi-length must be at least 1\n", EXTENSION_FULL);
        exit(1);
    }

    // create object
    FIRPFBCH2() q = (FIRPFBCH2()) malloc(sizeof(struct FIRPFBCH2(_s)));
    q->type       = _type;  // channelizer type (e.g. LIQUID_ANALYZER)
    q->M          = _M;     // number of channels
    q->m          = _m;     // prototype filter semi-length

    // compute derived values
    q->h_len      = 2*q->M*q->m;
    q->M2         = q->M / 2;

    // generate bank of sub-samped filters
    q->dp = (DOTPROD()*) malloc((q->M)*sizeof(DOTPROD()));
    unsigned int i;
    unsigned int n;
    unsigned int h_sub_len = 2 * q->m;
    TC h_sub[h_sub_len];
    for (i=0; i<q->M; i++) {
        // sub-sample prototype filter, loading coefficients in reverse order
        for (n=0; n<h_sub_len; n++) {
            h_sub[h_sub_len-n-1] = q->h[i + n*(q->M)];
        }
        // create dotprod object
        q->dp[i] = DOTPROD(_create)(h_sub,h_sub_len);
    }

    // create fft plan
    // TODO : use fftw_malloc if HAVE_FFTW3_H
    q->X = (T*) malloc((q->M)*sizeof(T));   // IFFT input
    q->x = (T*) malloc((q->M)*sizeof(T));   // IFFT output
    q->ifft = FFT_CREATE_PLAN(q->M, q->X, q->x, FFT_DIR_BACKWARD, FFT_METHOD);

    // return filterbank object
    return q;
}

// create firpfbch2 object using Kaiser window prototype
//  _type   :   channelizer type (e.g. LIQUID_ANALYZER)
//  _M      :   number of channels (must be even)
//  _m      :   prototype filter semi-lenth, length=2*M*m+1
//  _As     :   filter stop-band attenuation [dB]
FIRPFBCH2() FIRPFBCH2(_create_kaiser)(int          _type,
                                      unsigned int _M,
                                      unsigned int _m,
                                      float        _As)
{
    // validate input
    if (_type != LIQUID_ANALYZER && _type != LIQUID_SYNTHESIZER) {
        fprintf(stderr,"error: firpfbch2_%s_create_kaiser(), invalid type %d\n", EXTENSION_FULL, _type);
        exit(1);
    } else if (_M < 2 || _M % 2) {
        fprintf(stderr,"error: firpfbch2_%s_create_kaiser(), number of channels must be greater than 2 and even\n", EXTENSION_FULL);
        exit(1);
    } else if (_m < 1) {
        fprintf(stderr,"error: firpfbch2_%s_create_kaiser(), filter semi-length must be at least 1\n", EXTENSION_FULL);
        exit(1);
    }

    // design prototype filter
    unsigned int h_len = 2*_M*_m+1;
    float * hf = (float*)malloc(h_len*sizeof(float));

    // filter cut-off frequency
    float fc = (_type == LIQUID_ANALYZER) ? 1.0f/(float)_M : 0.5f/(float)_M;

    // compute filter coefficients
    liquid_firdes_kaiser(h_len, fc, _As, 0.0f, hf);

    // convert to type-specific array
    TC * h = (TC*) malloc(h_len * sizeof(TC));
    unsigned int i;
    for (i=0; i<h_len; i++)
        h[i] = (TC) hf[i];

    // create filterbank channelizer object
    FIRPFBCH2() q = FIRPFBCH2(_create)(_type, _M, _m, h);

    // free prototype filter coefficients
    free(hf);
    free(h);

    // return object
    return q;
}

// destroy firpfbch2 object, freeing internal memory
void FIRPFBCH2(_destroy)(FIRPFBCH2() _q)
{
    unsigned int i;

    // free dotprod objects
    for (i=0; i<_q->M; i++)
        DOTPROD(_destroy)(_q->dp[i]);
    free(_q->dp);

    // free transform object and arrays
    FFT_DESTROY_PLAN(_q->ifft);
    free(_q->X);
    free(_q->x);

    // free main object memory
    free(_q);
}

// reset firpfbch2 object internals
void FIRPFBCH2(_reset)(FIRPFBCH2() _q)
{
}

// print firpfbch2 object internals
void FIRPFBCH2(_print)(FIRPFBCH2() _q)
{
}

// execute filterbank channelizer (analyzer)
//  _x      :   channelizer input,  [size: M/2 x 1]
//  _y      :   channelizer output, [size: M   x 1]
void FIRPFBCH2(_execute_analyzer)(FIRPFBCH2() _q,
                                  TI *        _x,
                                  TO *        _y)
{
}

// execute filterbank channelizer (synthesizer)
//  _x      :   channelizer input,  [size: M   x 1]
//  _y      :   channelizer output, [size: M/2 x 1]
void FIRPFBCH2(_execute_synthesizer)(FIRPFBCH2() _q,
                                     TI *        _x,
                                     TO *        _y)
{
}

// execute filterbank channelizer
// LIQUID_ANALYZER:     input: M/2, output: M
// LIQUID_SYNTHESIZER:  input: M,   output: M/2
//  _x      :   channelizer input
//  _y      :   channelizer output
void FIRPFBCH2(_execute)(FIRPFBCH2() _q,
                         TI *        _x,
                         TO *        _y)
{
    switch (_q->type) {
    case LIQUID_ANALYZER:
        FIRPFBCH2(_execute_analyzer)(_q, _x, _y);
        return;
    case LIQUID_SYNTHESIZER:
        FIRPFBCH2(_execute_synthesizer)(_q, _x, _y);
        return;
    default:
        fprintf(stderr,"error: firpfbch2_%s_execute(), invalid type\n", EXTENSION_FULL);
        exit(1);
    }
}

