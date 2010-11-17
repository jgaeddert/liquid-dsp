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
// firpfbch.c
//
// finite impulse response polyphase filterbank channelizer
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "liquid.internal.h"

// firpfbch object structure definition
struct FIRPFBCH(_s) {
    int type;                   // synthesis/analysis
    unsigned int num_channels;  // number of channels
    unsigned int p;             // filter length (symbols)
    TO * x;                     // time-domain buffer
    TO * X;                     // freq-domain buffer

    // filter
    unsigned int h_len;         // filter length
    TC * h;                     // filter coefficients
    unsigned int filter_index;
    
    // create separate bank of dotprod and window objects
    DOTPROD() * dp;             // dot product object array
    WINDOW() * w;               // window buffer object array

    // fft plan
    FFT_PLAN fft;               // fft|ifft object
};

// create FIR polyphase filterbank channelizer object
//  _type           :   channelizer type (FIRPFBCH_ANALYZER | FIRPFBCH_SYNTHESIZER)
//  _num_channels   :   number of channels
//  _p              :   filter length (symbols)
//  _h              :   filter coefficients, [size: _num_channels*_p x 1]
FIRPFBCH() FIRPFBCH(_create)(int _type,
                             unsigned int _num_channels,
                             unsigned int _p,
                             TC * _h)
{
    // validate input
    if (_type != FIRPFBCH_ANALYZER && _type != FIRPFBCH_SYNTHESIZER) {
        printf("error: firpfbch_xxxt_create(), invalid type %d\n", _type);
        exit(1);
    } else if (_num_channels == 0) {
        printf("error: firpfbch_xxxt_create(), number of channels must be greater than 0\n");
        exit(1);
    } else if (_p == 0) {
        printf("error: firpfbch_xxxt_create(), invalid filter size (must be greater than 0)\n");
        exit(1);
    }

    // create object
    FIRPFBCH() q = (FIRPFBCH()) malloc(sizeof(struct FIRPFBCH(_s)));
    q->type         = _type;
    q->num_channels = _num_channels;
    q->p            = _p;

    // derived values
    q->h_len = q->num_channels * q->p;

    // create bank of filters
    q->dp   = (DOTPROD()*) malloc((q->num_channels)*sizeof(DOTPROD()));
    q->w    = (WINDOW()*) malloc((q->num_channels)*sizeof(WINDOW()));

    // copy filter coefficients
    q->h = (TC*) malloc((q->h_len)*sizeof(TC));
    unsigned int i;
    for (i=0; i<q->h_len; i++)
        q->h[i] = _h[i];

    // generate bank of sub-samped filters
    unsigned int n;
    unsigned int h_sub_len = q->p;
    TC h_sub[h_sub_len];
    for (i=0; i<q->num_channels; i++) {
        // sub-sample prototype filter, loading coefficients in reverse order
        for (n=0; n<h_sub_len; n++) {
            h_sub[h_sub_len-n-1] = q->h[i + n*(q->num_channels)];
        }
        // create window buffer and dotprod object (coefficients
        // loaded in reverse order)
        q->dp[i] = DOTPROD(_create)(h_sub,h_sub_len);
        q->w[i]  = WINDOW(_create)(h_sub_len);
    }

    // allocate memory for buffers
    // TODO : use fftw_malloc if HAVE_FFTW3_H
    q->x = (T*) malloc((q->num_channels)*sizeof(T));
    q->X = (T*) malloc((q->num_channels)*sizeof(T));

    // create fft plan
    if (q->type == FIRPFBCH_ANALYZER)
        q->fft = FFT_CREATE_PLAN(q->num_channels, q->X, q->x, FFT_DIR_FORWARD, FFT_METHOD);
    else
        q->fft = FFT_CREATE_PLAN(q->num_channels, q->X, q->x, FFT_DIR_BACKWARD, FFT_METHOD);

    // clear filterbank object
    FIRPFBCH(_clear)(q);

    // return filterbank object
    return q;
}

// destroy firpfbch object
void FIRPFBCH(_destroy)(FIRPFBCH() _q)
{
    unsigned int i;

    // free dot product, window objects and arrays
    for (i=0; i<_q->num_channels; i++) {
        DOTPROD(_destroy)(_q->dp[i]);
        WINDOW(_destroy)(_q->w[i]);
    }
    free(_q->dp);
    free(_q->w);

    // free transform object
    FFT_DESTROY_PLAN(_q->fft);

    // free additional arrays
    free(_q->h);
    free(_q->x);
    free(_q->X);

    // free main object memory
    free(_q);
}

// clear firpfbch object internals
void FIRPFBCH(_clear)(FIRPFBCH() _q)
{
    unsigned int i;
    for (i=0; i<_q->num_channels; i++) {
        WINDOW(_clear)(_q->w[i]);
        _q->x[i] = 0;
        _q->X[i] = 0;
    }
    _q->filter_index = _q->num_channels-1;
}

// print firpfbch object
void FIRPFBCH(_print)(FIRPFBCH() _q)
{
    printf("firpfbch [%u channels]:\n", _q->num_channels);
}

// 
// SYNTHESIZER
//

void FIRPFBCH(_synthesizer_execute)(FIRPFBCH() _q,
                                    TI * _x,
                                    TO * _y)
{
    unsigned int i;

    memmove(_q->X, _x, _q->num_channels*sizeof(TI));

    // execute inverse DFT, store result in buffer 'x'
    FFT_EXECUTE(_q->fft);

    // push samples into filter bank and execute
    T * r;      // read pointer
    for (i=0; i<_q->num_channels; i++) {
        WINDOW(_push)(_q->w[i], _q->x[i]);
        WINDOW(_read)(_q->w[i], &r);
        DOTPROD(_execute)(_q->dp[i], r, &_y[i]);

        // normalize by DFT scaling factor
        //_y[i] /= (float) (_q->num_channels);
    }
}

// 
// ANALYZER
//

// execute filterbank as analyzer on block of samples
//  _q      :   filterbank channelizer object
//  _x      :   input time series, [size: num_channels x 1]
//  _y      :   channelized output, [size: num_channels x 1]
void FIRPFBCH(_analyzer_execute)(FIRPFBCH() _q,
                                 TI * _x,
                                 TO * _y)
{
    unsigned int i;

    // push samples into buffers
    for (i=0; i<_q->num_channels; i++)
        FIRPFBCH(_analyzer_push)(_q, _x[i]);

    // execute analysis filters on the given input
    FIRPFBCH(_analyzer_run)(_q, _y);
}

// push single sample into analysis filterbank, updating index
// counter appropriately
//  _q      :   filterbank channelizer object
//  _x      :   input sample
void FIRPFBCH(_analyzer_push)(FIRPFBCH() _q,
                              TI _x)
{
    // push sample into filter
    WINDOW(_push)(_q->w[_q->filter_index], _x);

    // decrement filter index
    _q->filter_index = (_q->filter_index + _q->num_channels - 1) % _q->num_channels;
}

// run filterbank analyzer dot products, DFT
//  _q      :   filterbank channelizer object
//  _y      :   output array, [size: num_channels x 1]
void FIRPFBCH(_analyzer_run)(FIRPFBCH() _q,
                             TO * _y)
{
    unsigned int i;

    // execute filter outputs, reversing order of output (not
    // sure why this is necessary)
    T * r;  // read pointer
    for (i=0; i<_q->num_channels; i++) {
        WINDOW(_read)(_q->w[i], &r);
        DOTPROD(_execute)(_q->dp[i], r, &_q->X[_q->num_channels-i-1]);
    }

    // execute DFT, store result in buffer 'x'
    FFT_EXECUTE(_q->fft);

    // move to output array
    memmove(_y, _q->x, _q->num_channels*sizeof(TO));
}


