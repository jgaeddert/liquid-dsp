/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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
// Finite impulse response filter design
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

// Design FIR using kaiser window
//  _n      : filter length
//  _fc     : cutoff frequency
//  _slsl   : sidelobe suppression level (dB attenuation)
//  _h      : output coefficient buffer
void fir_kaiser_window(unsigned int _n, float _fc, float _slsl, float _mu, float *_h) {
    // chooise kaiser beta parameter (approximate)
    // from:
    //  P.P. Vaidyanathan, "Multirate Systems and Filter Banks
    _slsl = fabsf(_slsl);
    float beta;
    if (_slsl > 50.0f)
        beta = 0.1102f*(_slsl - 8.7f);
    else if (_slsl > 21.0f)
        beta = 0.5842*powf(_slsl - 21, 0.4f) + 0.07886f*(_slsl - 21);
    else
        beta = 0.0f;

    float t, h1, h2; 
    unsigned int i;
    for (i=0; i<_n; i++) {
        t = (float)i - (float)(_n-1)/2 + _mu;
     
        // sinc prototype
        h1 = sincf(_fc*t);

        // kaiser window
        h2 = kaiser(i,_n,beta,_mu);

        //printf("t = %f, h1 = %f, h2 = %f\n", t, h1, h2);

        // composite
        _h[i] = h1*h2;
    }   
}


// Design FIR doppler filter
//  _n      : filter length
//  _fd     : normalized doppler frequency (0 < _fd < 0.5)
//  _K      : Rice fading factor (K >= 0)
//  _theta  : LoS component angle of arrival
//  _h      : output coefficient buffer
void fir_design_doppler(unsigned int _n, float _fd, float _K, float _theta, float *_h)
{
    float t, J, r, w;
    float beta = 4; // kaiser window parameter
    unsigned int i;
    for (i=0; i<_n; i++) {
        // time sample
        t = (float)i - (float)(_n-1)/2;

        // Bessel
        J = 1.5*besselj_0(fabsf(2*M_PI*_fd*t));

        // Rice-K component
        r = 1.5*_K/(_K+1)*cosf(2*M_PI*_fd*t*cosf(_theta));

        // Window
        w = kaiser(i, _n, beta, 0);

        // composite
        _h[i] = (J+r)*w;

        //printf("t=%f, J=%f, r=%f, w=%f\n", t, J, r, w);
    }
}

// Design optimum FIR root-nyquist filter
//  _n      : filter length
//  _k      : samples/symbol
//  _beta   : excess bandwidth factor
void fir_design_optim_root_nyquist(unsigned int _n, unsigned int _k, float _slsl, float *_h)
{
    // validate inputs:
    //    _k >= 2
    //    _slsl < 0

    // begin with prototype
    //float fc = 1/((float)_k);
    //fir_design_windowed_sinc(_n, fc, _slsl, _h);

    // begin optimization:
}


