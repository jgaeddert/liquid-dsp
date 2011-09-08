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
// Finite impulse response GMSK transmit/receive filter design
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

// Design GMSK transmit filter
//  _k      : samples/symbol
//  _m      : symbol delay
//  _beta   : rolloff factor (0 < beta <= 1)
//  _dt     : fractional sample delay
//  _h      : output coefficient buffer (length: 2*k*m+1)
void liquid_firdes_gmsktx(unsigned int _k,
                          unsigned int _m,
                          float _beta,
                          float _dt,
                          float * _h)
{
    // validate input
    if ( _k < 1 ) {
        fprintf(stderr,"error: liquid_firdes_gmsktx(): k must be greater than 0\n");
        exit(1);
    } else if ( _m < 1 ) {
        fprintf(stderr,"error: liquid_firdes_gmsktx(): m must be greater than 0\n");
        exit(1);
    } else if ( (_beta < 0.0f) || (_beta > 1.0f) ) {
        fprintf(stderr,"error: liquid_firdes_gmsktx(): beta must be in [0,1]\n");
        exit(1);
    } else;

    // derived values
    unsigned int h_len = 2*_k*_m+1;

    // compute filter coefficients
    unsigned int i;
    float t;
    float c0 = 1.0f / sqrtf(logf(2.0f));
    for (i=0; i<h_len; i++) {
        t = (float)i/(float)(_k)-(float)(_m) + _dt;

        _h[i] = liquid_Qf(2*M_PI*_beta*(t-0.5f)*c0) -
                liquid_Qf(2*M_PI*_beta*(t+0.5f)*c0);
    }

    // normalize filter coefficients such that the filter's
    // integral is pi/2
    float e = 0.0f;
    for (i=0; i<h_len; i++)
        e += _h[i];
    for (i=0; i<h_len; i++)
        _h[i] *= M_PI / (2.0f * e);
}

// Design GMSK receive filter
//  _k      : samples/symbol
//  _m      : symbol delay
//  _beta   : rolloff factor (0 < beta <= 1)
//  _dt     : fractional sample delay
//  _h      : output coefficient buffer (length: 2*k*m+1)
void liquid_firdes_gmskrx(unsigned int _k,
                          unsigned int _m,
                          float _beta,
                          float _dt,
                          float * _h)
{
    // TODO : compute ideal equalized filter response
    // for now, simply use same as transmit filter
    liquid_firdes_gmsktx(_k, _m, _beta, _dt, _h);
}

