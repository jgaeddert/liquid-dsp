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
// Design raised-cosine filter
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

// Design Nyquist raised-cosine filter
//  _k      : samples/symbol
//  _m      : symbol delay
//  _beta   : rolloff factor (0 < beta <= 1)
//  _dt     : fractional sample delay
//  _h      : output coefficient buffer (length: 2*k*m+1)
void liquid_firdes_rcos(unsigned int _k,
                        unsigned int _m,
                        float _beta,
                        float _dt,
                        float * _h)
{
    if ( _k < 1 ) {
        fprintf(stderr,"error: liquid_firdes_rcos(): k must be greater than 0\n");
        exit(1);
    } else if ( _m < 1 ) {
        fprintf(stderr,"error: liquid_firdes_rcos(): m must be greater than 0\n");
        exit(1);
    } else if ( (_beta < 0.0f) || (_beta > 1.0f) ) {
        fprintf(stderr,"error: liquid_firdes_rcos(): beta must be in [0,1]\n");
        exit(1);
    } else;

    unsigned int n;
    float z, t1, t2, t3;

    float nf, kf, mf;

    unsigned int h_len = 2*_k*_m + 1;

    // Calculate filter coefficients
    for (n=0; n<h_len; n++) {
        nf = (float) n;
        kf = (float) _k;
        mf = (float) _m;

        z = (nf+_dt)/kf-mf;
        t1 = cosf(_beta*M_PI*z);
        t2 = sincf(z);
        t3 = 1 - 4.0f*_beta*_beta*z*z;

        // check for special condition where 4*_beta^2*z^2 equals 1
        if ( fabsf(t3) < 1e-3f )
            _h[n] = sinf(M_PI/(2.0*_beta))*_beta*0.5f;
        else
            _h[n] = t1*t2/t3;
    }
}

