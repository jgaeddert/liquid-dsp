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

    // fft plan
    FFT_PLAN fft;               // inverse FFT object
    TO * X;                     // IFFT input array  [size: M x 1]
    TO * x;                     // IFFT output array [size: M x 1]
};

// create firpfbch2 object
//  _type   :   channelizer type (e.g. LIQUID_ANALYZER)
//  _M      :   number of channels (must be even)
//  _h      :   prototype filter coefficient array
//  _h_len  :   number of coefficients
FIRPFBCH2() FIRPFBCH2(_create)(int          _type,
                               unsigned int _M,
                               TC *         _h,
                               unsigned int _h_len)
{
    // validate input
    if (_type != LIQUID_ANALYZER && _type != LIQUID_SYNTHESIZER) {
        fprintf(stderr,"error: firpfbch2_%s_create(), invalid type %d\n", EXTENSION_FULL, _type);
        exit(1);
    } else if (_M < 2 || _M % 2) {
        fprintf(stderr,"error: firpfbch2_%s_create(), number of channels must be greater than 2 and even\n", EXTENSION_FULL);
        exit(1);
    } else if (_h_len % _M) {
        fprintf(stderr,"error: firpfbch2_%s_create(), invalid filter size (must be greater than 0)\n", EXTENSION_FULL);
        exit(1);
    }

    // create object
    FIRPFBCH2() q = (FIRPFBCH2()) malloc(sizeof(struct FIRPFBCH2(_s)));
    q->type       = _type;
    q->M          = _M;

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
    return NULL;
}

// destroy firpfbch2 object, freeing internal memory
void FIRPFBCH2(_destroy)(FIRPFBCH2() _q)
{
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

