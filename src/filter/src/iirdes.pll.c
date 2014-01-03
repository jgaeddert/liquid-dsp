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
// 2nd-order iir (infinite impulse response) phase-locked loop filter design
//
// References:
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "liquid.internal.h"

// design 2nd-order IIR filter (active lag)
//          1 + t2 * s
//  F(s) = ------------
//          1 + t1 * s
//
//  _w      :   filter bandwidth
//  _zeta   :   damping factor (1/sqrt(2) suggested)
//  _K      :   loop gain (1000 suggested)
//  _b      :   output feed-forward coefficients [size: 3 x 1]
//  _a      :   output feed-back coefficients [size: 3 x 1]
void iirdes_pll_active_lag(float _w,
                           float _zeta,
                           float _K,
                           float * _b,
                           float * _a)
{
    // validate input
    if (_w <= 0.0f) {
        fprintf(stderr,"error: iirdes_pll_active_lag(), bandwidth must be greater than 0\n");
        exit(1);
    } else if (_zeta <= 0.0f) {
        fprintf(stderr,"error: iirdes_pll_active_lag(), damping factor must be greater than 0\n");
        exit(1);
    } else if (_K <= 0.0f) {
        fprintf(stderr,"error: iirdes_pll_active_lag(), gain must be greater than 0\n");
        exit(1);
    }

    float wn = _w;                  // natural frequency
    float t1 = _K/(wn*wn);          // 
    float t2 = 2*_zeta/wn - 1/_K;   //

    _b[0] = 2*_K*(1.+t2/2.0f);
    _b[1] = 2*_K*2.;
    _b[2] = 2*_K*(1.-t2/2.0f);

    _a[0] =  1 + t1/2.0f;
    _a[1] = -t1;
    _a[2] = -1 + t1/2.0f;
}

// design 2nd-order IIR filter (active PI)
//          1 + t2 * s
//  F(s) = ------------
//           t1 * s
//
//  _w      :   filter bandwidth
//  _zeta   :   damping factor (1/sqrt(2) suggested)
//  _K      :   loop gain (1000 suggested)
//  _b      :   output feed-forward coefficients [size: 3 x 1]
//  _a      :   output feed-back coefficients [size: 3 x 1]
void iirdes_pll_active_PI(float _w,
                          float _zeta,
                          float _K,
                          float * _b,
                          float * _a)
{
    // validate input
    if (_w <= 0.0f) {
        fprintf(stderr,"error: iirdes_pll_active_lag(), bandwidth must be greater than 0\n");
        exit(1);
    } else if (_zeta <= 0.0f) {
        fprintf(stderr,"error: iirdes_pll_active_lag(), damping factor must be greater than 0\n");
        exit(1);
    } else if (_K <= 0.0f) {
        fprintf(stderr,"error: iirdes_pll_active_lag(), gain must be greater than 0\n");
        exit(1);
    }

    // loop filter (active lag)
    float wn = _w;          // natural frequency
    float t1 = _K/(wn*wn);  //
    float t2 = 2*_zeta/wn;  //

    _b[0] = 2*_K*(1.+t2/2.0f);
    _b[1] = 2*_K*2.;
    _b[2] = 2*_K*(1.-t2/2.0f);

    _a[0] =  t1/2.0f;
    _a[1] = -t1;
    _a[2] =  t1/2.0f;
}


