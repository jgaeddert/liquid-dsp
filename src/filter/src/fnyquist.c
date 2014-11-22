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
// Flipped Nyquist/root-Nyquist filter designs
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

// Design flipped Nyquist/root-Nyquist filter
//  _type   : filter type (e.g. LIQUID_FIRFILT_FEXP)
//  _root   : square-root Nyquist filter?
//  _k      : samples/symbol
//  _m      : symbol delay
//  _beta   : rolloff factor (0 < beta <= 1)
//  _dt     : fractional sample delay
//  _h      : output coefficient buffer (length: 2*k*m+1)
void liquid_firdes_fnyquist(liquid_firfilt_type _type,
                            int                 _root,
                            unsigned int        _k,
                            unsigned int        _m,
                            float               _beta,
                            float               _dt,
                            float *             _h)
{
    // validate input
    if ( _k < 1 ) {
        fprintf(stderr,"error: liquid_firdes_fnyquist(): k must be greater than 0\n");
        exit(1);
    } else if ( _m < 1 ) {
        fprintf(stderr,"error: liquid_firdes_fnyquist(): m must be greater than 0\n");
        exit(1);
    } else if ( (_beta < 0.0f) || (_beta > 1.0f) ) {
        fprintf(stderr,"error: liquid_firdes_fnyquist(): beta must be in [0,1]\n");
        exit(1);
    } else;

    unsigned int i;

    // derived values
    unsigned int h_len = 2*_k*_m+1;   // filter length

    float H_prime[h_len];   // frequency response of Nyquist filter (real)
    float complex H[h_len]; // frequency response of Nyquist filter
    float complex h[h_len]; // impulse response of Nyquist filter

    // compute Nyquist filter frequency response
    switch (_type) {
    case LIQUID_FIRFILT_FEXP:
        liquid_firdes_fexp_freqresponse(_k, _m, _beta, H_prime);
        break;
    case LIQUID_FIRFILT_FSECH:
        liquid_firdes_fsech_freqresponse(_k, _m, _beta, H_prime);
        break;
    case LIQUID_FIRFILT_FARCSECH:
        liquid_firdes_farcsech_freqresponse(_k, _m, _beta, H_prime);
        break;
    default:
        fprintf(stderr,"error: liquid_firdes_fnyquist(), unknown/unsupported filter type\n");
        exit(1);
    }

    // copy result to fft input buffer, computing square root
    // if required
    for (i=0; i<h_len; i++)
        H[i] = _root ? sqrtf(H_prime[i]) : H_prime[i];

    // compute ifft
    fft_run(h_len, H, h, LIQUID_FFT_BACKWARD, 0);
    
    // copy shifted, scaled response
    for (i=0; i<h_len; i++)
        _h[i] = crealf( h[(i+_k*_m+1)%h_len] ) * (float)_k / (float)(h_len);
}

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
    // compute resonse using generic function
    liquid_firdes_fnyquist(LIQUID_FIRFILT_FEXP, 0, _k, _m, _beta, _dt, _h);
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
    // compute resonse using generic function
    liquid_firdes_fnyquist(LIQUID_FIRFILT_FEXP, 1, _k, _m, _beta, _dt, _h);
}

// flipped exponential frequency response
void liquid_firdes_fexp_freqresponse(unsigned int _k,
                                     unsigned int _m,
                                     float        _beta,
                                     float *      _H)
{
    // TODO : validate input

    unsigned int i;

    unsigned int h_len = 2*_k*_m + 1;

    float f0 = 0.5f*(1.0f - _beta) / (float)_k;
    float f1 = 0.5f*(1.0f        ) / (float)_k;
    float f2 = 0.5f*(1.0f + _beta) / (float)_k;

    float B     = 0.5f/(float)_k;
    float gamma = logf(2.0f)/(_beta*B);

    // compute frequency response of Nyquist filter
    for (i=0; i<h_len; i++) {
        float f = (float)i / (float)h_len;
        if (f > 0.5f) f = f - 1.0f;

        // enforce even symmetry
        f = fabsf(f);

        if ( f < f0 ) {
            // pass band
            _H[i] = 1.0f;
        } else if (f > f0 && f < f2) {
            // transition band
            if ( f < f1) {
                _H[i] = expf(gamma*(B*(1-_beta) - f));
            } else {
                _H[i] = 1.0f - expf(gamma*(f - (1+_beta)*B));
            }
        } else {
            // stop band
            _H[i] = 0.0f;
        }
    }
}

// Design fsech Nyquist filter
//  _k      : samples/symbol
//  _m      : symbol delay
//  _beta   : rolloff factor (0 < beta <= 1)
//  _dt     : fractional sample delay
//  _h      : output coefficient buffer (length: 2*k*m+1)
void liquid_firdes_fsech(unsigned int _k,
                         unsigned int _m,
                         float _beta,
                         float _dt,
                         float * _h)
{
    // compute resonse using generic function
    liquid_firdes_fnyquist(LIQUID_FIRFILT_FSECH, 0, _k, _m, _beta, _dt, _h);
}

// Design fsech square-root Nyquist filter
//  _k      : samples/symbol
//  _m      : symbol delay
//  _beta   : rolloff factor (0 < beta <= 1)
//  _dt     : fractional sample delay
//  _h      : output coefficient buffer (length: 2*k*m+1)
void liquid_firdes_rfsech(unsigned int _k,
                          unsigned int _m,
                          float _beta,
                          float _dt,
                          float * _h)
{
    // compute resonse using generic function
    liquid_firdes_fnyquist(LIQUID_FIRFILT_FSECH, 1, _k, _m, _beta, _dt, _h);
}

// flipped exponential frequency response
void liquid_firdes_fsech_freqresponse(unsigned int _k,
                                     unsigned int _m,
                                     float        _beta,
                                     float *      _H)
{
    // TODO : validate input

    unsigned int i;

    unsigned int h_len = 2*_k*_m + 1;

    float f0 = 0.5f*(1.0f - _beta) / (float)_k;
    float f1 = 0.5f*(1.0f        ) / (float)_k;
    float f2 = 0.5f*(1.0f + _beta) / (float)_k;

    float B     = 0.5f/(float)_k;
    float gamma = logf(sqrtf(3.0f) + 2.0f) / (_beta*B);

    // compute frequency response of Nyquist filter
    for (i=0; i<h_len; i++) {
        float f = (float)i / (float)h_len;
        if (f > 0.5f) f = f - 1.0f;

        // enforce even symmetry
        f = fabsf(f);

        if ( f < f0 ) {
            // pass band
            _H[i] = 1.0f;
        } else if (f > f0 && f < f2) {
            // transition band
            if ( f < f1) {
                _H[i] = 1.0f / coshf(gamma*(f - B*(1-_beta)));
            } else {
                _H[i] = 1.0f - 1.0f / coshf(gamma*(B*(1+_beta) - f));
            }
        } else {
            // stop band
            _H[i] = 0.0f;
        }
    }
}

// Design farcsech Nyquist filter
//  _k      : samples/symbol
//  _m      : symbol delay
//  _beta   : rolloff factor (0 < beta <= 1)
//  _dt     : fractional sample delay
//  _h      : output coefficient buffer (length: 2*k*m+1)
void liquid_firdes_farcsech(unsigned int _k,
                            unsigned int _m,
                            float _beta,
                            float _dt,
                            float * _h)
{
    // compute resonse using generic function
    liquid_firdes_fnyquist(LIQUID_FIRFILT_FARCSECH, 0, _k, _m, _beta, _dt, _h);
}

// Design farcsech square-root Nyquist filter
//  _k      : samples/symbol
//  _m      : symbol delay
//  _beta   : rolloff factor (0 < beta <= 1)
//  _dt     : fractional sample delay
//  _h      : output coefficient buffer (length: 2*k*m+1)
void liquid_firdes_rfarcsech(unsigned int _k,
                             unsigned int _m,
                             float _beta,
                             float _dt,
                             float * _h)
{
    // compute resonse using generic function
    liquid_firdes_fnyquist(LIQUID_FIRFILT_FARCSECH, 1, _k, _m, _beta, _dt, _h);
}

// hyperbolic arc-secant
float liquid_asechf(float _z)
{
    if (_z <= 0.0f || _z > 1.0f) {
        fprintf(stderr,"warning: liquid_asechf(), input out of range\n");
        return 0.0f;
    }

    float z_inv = 1.0f / _z;

    return logf( sqrtf(z_inv - 1.0f)*sqrtf(z_inv + 1.0f) + z_inv );
}

// flipped exponential frequency response
void liquid_firdes_farcsech_freqresponse(unsigned int _k,
                                         unsigned int _m,
                                         float        _beta,
                                         float *      _H)
{
    // TODO : validate input

    unsigned int i;

    unsigned int h_len = 2*_k*_m + 1;

    float f0 = 0.5f*(1.0f - _beta) / (float)_k;
    float f1 = 0.5f*(1.0f        ) / (float)_k;
    float f2 = 0.5f*(1.0f + _beta) / (float)_k;

    float B     = 0.5f/(float)_k;
    float gamma = logf(sqrtf(3.0f) + 2.0f) / (_beta*B);
    float zeta  = 1.0f / (2.0f * _beta * B);

    // compute frequency response of Nyquist filter
    for (i=0; i<h_len; i++) {
        float f = (float)i / (float)h_len;
        if (f > 0.5f) f = f - 1.0f;

        // enforce even symmetry
        f = fabsf(f);

        if ( f < f0 ) {
            // pass band
            _H[i] = 1.0f;
        } else if (f > f0 && f < f2) {
            // transition band
            if ( f < f1) {
                _H[i] = 1.0f - (zeta/gamma)*liquid_asechf(zeta*(B*(1+_beta) - f));
            } else {
                _H[i] = (zeta/gamma)*liquid_asechf(zeta*(f - B*(1-_beta)));
            }
        } else {
            // stop band
            _H[i] = 0.0f;
        }
    }
}

