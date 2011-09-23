/*
 * Copyright (c) 2011 Joseph Gaeddert
 * Copyright (c) 2011 Virginia Polytechnic Institute & State University
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
// Flipped exponential filter
//
// References:
//   [Beaulieu:2001]
//   [Assalini:2004]
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "liquid.internal.h"

// Design fexp Nyquist filter
//  _k      : samples/symbol
//  _m      : symbol delay
//  _beta   : rolloff factor (0 < beta <= 1)
//  _dt     : fractional sample delay
//  _h      : output coefficient buffer (length: 2*k*m+1)
void liquid_firdes_fexp(unsigned int _k,
                        unsigned int _m,
                        float _beta,
                        float _dt,
                        float * _h)
{
    // validate input
    if ( _k < 1 ) {
        fprintf(stderr,"error: liquid_firdes_fexp(): k must be greater than 0\n");
        exit(1);
    } else if ( _m < 1 ) {
        fprintf(stderr,"error: liquid_firdes_fexp(): m must be greater than 0\n");
        exit(1);
    } else if ( (_beta < 0.0f) || (_beta > 1.0f) ) {
        fprintf(stderr,"error: liquid_firdes_fexp(): beta must be in [0,1]\n");
        exit(1);
    } else;

    unsigned int i;

    // derived values
    unsigned int h_len = 2*_k*_m+1;   // filter length

    //
    // start of filter design procedure
    //

    float complex h_tx[h_len];      // impulse response of Nyquist filter
    float complex H_tx[h_len];      // frequency response of Nyquist filter

    // compute frequency response of Nyquist filter
    for (i=0; i<h_len; i++) {
        float f = (float)i / (float)h_len;
        if (f > 0.5f) f = f - 1.0f;

        H_tx[i] = liquid_firdes_fexp_freqresponse(f,_k,_beta);
    }

    // compute ifft and copy shifted, scaled response
    fft_run(h_len, H_tx, h_tx, FFT_REVERSE, 0);
    for (i=0; i<h_len; i++)
        _h[i] = crealf( h_tx[(i+_k*_m+1)%h_len] ) * (float)_k / (float)(h_len);

    //
    // end of filter design procedure
    //
}

// Design fexp square-root Nyquist filter
//  _k      : samples/symbol
//  _m      : symbol delay
//  _beta   : rolloff factor (0 < beta <= 1)
//  _dt     : fractional sample delay
//  _h      : output coefficient buffer (length: 2*k*m+1)
void liquid_firdes_rfexp(unsigned int _k,
                         unsigned int _m,
                         float _beta,
                         float _dt,
                         float * _h)
{
    // validate input
    if ( _k < 1 ) {
        fprintf(stderr,"error: liquid_firdes_rfexp(): k must be greater than 0\n");
        exit(1);
    } else if ( _m < 1 ) {
        fprintf(stderr,"error: liquid_firdes_rfexp(): m must be greater than 0\n");
        exit(1);
    } else if ( (_beta < 0.0f) || (_beta > 1.0f) ) {
        fprintf(stderr,"error: liquid_firdes_rfexp(): beta must be in [0,1]\n");
        exit(1);
    } else;
    
    unsigned int i;

    // derived values
    unsigned int h_len = 2*_k*_m+1;   // filter length

    //
    // start of filter design procedure
    //

    float H_prime[h_len];           // frequency response of Nyquist filter
    float complex h_tx[h_len];      // impulse response of square-root Nyquist filter
    float complex H_tx[h_len];      // frequency response of square-root Nyquist filter

    // compute frequency response of Nyquist filter
    for (i=0; i<h_len; i++) {
        float f = (float)i / (float)h_len;
        if (f > 0.5f) f = f - 1.0f;

        H_prime[i] = liquid_firdes_fexp_freqresponse(f,_k,_beta);
    }

    // compute square-root response, copy to fft input
    for (i=0; i<h_len; i++)
        H_tx[i] = sqrtf(H_prime[i]);

    // compute ifft and copy response
    fft_run(h_len, H_tx, h_tx, FFT_REVERSE, 0);
    for (i=0; i<h_len; i++)
        _h[i] = crealf( h_tx[(i+_k*_m+1)%h_len] ) * (float)_k / (float)(h_len);

    //
    // end of filter design procedure
    //
}

// flipped exponential frequency response
float liquid_firdes_fexp_freqresponse(float _f,
                                      unsigned int _k,
                                      float _beta)
{
    // TODO : validate input

    // enforce even symmetry
    float f = fabsf(_f);

    float f0 = 0.5f*(1.0f - _beta) / (float)_k;
    float f1 = 0.5f*(1.0f        ) / (float)_k;
    float f2 = 0.5f*(1.0f + _beta) / (float)_k;

    if ( f < f0 ) {
        return 1.0f;
    } else if (f > f0 && f < f2) {
        // transition band
        float B     = 0.5f/(float)_k;
        float gamma = logf(2.0f)/(_beta*B);
        if ( f < f1) {
            return expf(gamma*(B*(1-_beta) - f));
        } else {
            return 1.0f - expf(gamma*(f - (1+_beta)*B));
        }
    } else {
        return 0.0f;
    }

    return 0.0f;
}

