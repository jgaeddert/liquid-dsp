/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Virginia Polytechnic
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
// fft_dft.c : definitions for regular, slow DFTs
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "liquid.internal.h"

// create FFT plan for regular DFT
//  _nfft   :   FFT size
//  _x      :   input array [size: _nfft x 1]
//  _y      :   output array [size: _nfft x 1]
//  _dir    :   fft direction: {FFT_FORWARD, FFT_REVERSE}
//  _method :   fft method
FFT(plan) FFT(_create_plan_dft)(unsigned int _nfft,
                                TC *         _x,
                                TC *         _y,
                                int          _dir,
                                int          _flags)
{
    // allocate plan and initialize all internal arrays to NULL
    FFT(plan) q = (FFT(plan)) malloc(sizeof(struct FFT(plan_s)));

    q->nfft      = _nfft;
    q->x         = _x;
    q->y         = _y;
    q->flags     = _flags;
    q->kind      = LIQUID_FFT_DFT_1D;
    q->direction = (_dir == FFT_FORWARD) ? FFT_FORWARD : FFT_REVERSE;
    q->method    = LIQUID_FFT_METHOD_DFT;
        
    q->data.dft.twiddle = NULL;

    // check size, use specific codelet for small DFTs
    if      (q->nfft == 2) q->execute = FFT(_execute_dft_2);
    else if (q->nfft == 3) q->execute = FFT(_execute_dft_3);
    else if (q->nfft == 4) q->execute = FFT(_execute_dft_4);
    else if (q->nfft == 5) q->execute = FFT(_execute_dft_5);
    else if (q->nfft == 6) q->execute = FFT(_execute_dft_6);
    else if (q->nfft == 7) q->execute = FFT(_execute_dft_7);
    else if (q->nfft == 8) q->execute = FFT(_execute_dft_8);
    else {
        q->execute = FFT(_execute_dft);

        // initialize twiddle factors
        q->data.dft.twiddle = (TC *) malloc(q->nfft * sizeof(TC));
        
        unsigned int i;
        T d = (q->direction == FFT_FORWARD) ? -1.0 : 1.0;
        for (i=0; i<q->nfft; i++)
            q->data.dft.twiddle[i] = cexpf(_Complex_I*d*2*M_PI*(T)i / (T)(q->nfft));
    }

    return q;
}

// destroy FFT plan
void FFT(_destroy_plan_dft)(FFT(plan) _q)
{
    // free twiddle factors
    if (_q->data.dft.twiddle != NULL)
        free(_q->data.dft.twiddle);

    // free main object memory
    free(_q);
}

// execute DFT (slow but functionally correct)
void FFT(_execute_dft)(FFT(plan) _q)
{
    unsigned int i;
    unsigned int k;
    unsigned int nfft = _q->nfft;

    // DC value is sum of input
    _q->y[0] = _q->x[0];
    for (i=1; i<nfft; i++)
        _q->y[0] += _q->x[i];
    
    // compute remaining DFT values
    for (i=1; i<nfft; i++) {
        _q->y[i] = _q->x[0];
        for (k=1; k<nfft; k++) {
            _q->y[i] += _q->x[k] * _q->data.dft.twiddle[(i*k)%_q->nfft];
        }
    }
}

// 
// codelets for small DFTs
//

// 
void FFT(_execute_dft_2)(FFT(plan) _q)
{
    _q->y[0] = _q->x[0] + _q->x[1];
    _q->y[1] = _q->x[0] - _q->x[1];
}

//
void FFT(_execute_dft_3)(FFT(plan) _q)
{
    TC g  = -0.5f - _Complex_I*0.866025403784439; // sqrt(3)/2

    _q->y[0] = _q->x[0] + _q->x[1]          + _q->x[2];
    TC ta    = _q->x[0] + _q->x[1]*g        + _q->x[2]*conjf(g);
    TC tb    = _q->x[0] + _q->x[1]*conjf(g) + _q->x[2]*g;

    // set return values
    if (_q->direction == FFT_FORWARD) {
        _q->y[1] = ta;
        _q->y[2] = tb;
    } else {
        _q->y[1] = tb;
        _q->y[2] = ta;
    }
}

//
void FFT(_execute_dft_4)(FFT(plan) _q)
{
    TC yp;
    TC * x = _q->x;
    TC * y = _q->y;

    // index reversal
    y[0] = x[0];
    y[1] = x[2];
    y[2] = x[1];
    y[3] = x[3];

    // k0 = 0, k1=1
    yp = y[1];
    y[1] = y[0] - yp;
    y[0] = y[0] + yp;

    // k0 = 2, k1=3
    yp = y[3];
    y[3] = y[2] - yp;
    y[2] = y[2] + yp;

    // k0 = 0, k1=2
    yp = y[2];
    y[2] = y[0] - yp;
    y[0] = y[0] + yp;

    // k0 = 1, k1=3
    yp = cimagf(y[3]) - _Complex_I*crealf(y[3]);
    if (_q->direction == FFT_REVERSE)
        yp = -yp;
    y[3] = y[1] - yp;
    y[1] = y[1] + yp;
}

//
void FFT(_execute_dft_5)(FFT(plan) _q)
{
    TC * x = _q->x;
    TC * y = _q->y;

    // DC value is sum of inputs
    y[0] = x[0] + x[1] + x[2] + x[3] + x[4];

    // exp(-j*2*pi*1/5)
    TC g0 =  0.309016994374947 - 0.951056516295154*_Complex_I;

    // exp(-j*2*pi*2/5)
    TC g1 = -0.809016994374947 - 0.587785252292473*_Complex_I;

    if (_q->direction == FFT_REVERSE) {
        g0 = conjf(g0);
        g1 = conjf(g1);
    }
    TC g0_conj = conjf(g0);
    TC g1_conj = conjf(g1);

    y[1] = x[0] + x[1]*g0      + x[2]*g1      + x[3]*g1_conj + x[4]*g0_conj;
    y[2] = x[0] + x[1]*g1      + x[2]*g0_conj + x[3]*g0      + x[4]*g1_conj;
    y[3] = x[0] + x[1]*g1_conj + x[2]*g0      + x[3]*g0_conj + x[4]*g1;
    y[4] = x[0] + x[1]*g0_conj + x[2]*g1_conj + x[3]*g1      + x[4]*g0;
}

//
void FFT(_execute_dft_6)(FFT(plan) _q)
{
    TC * x = _q->x;
    TC * y = _q->y;

    // DC value is sum of inputs
    y[0] = x[0] + x[1] + x[2] + x[3] + x[4] + x[5];

    // exp(-j*2*pi*1/6) = 1/2 - j*sqrt(3)/2
    TC g = 0.5 - 0.866025403784439*_Complex_I;

    TC g1, g2, g4, g5;

    if (_q->direction == FFT_FORWARD) {
        g1 =        g;  // exp(-j*2*pi*1/6)
        g2 = -conjf(g); // exp(-j*2*pi*2/6)
        g4 =       -g;  // exp(-j*2*pi*4/6)
        g5 =  conjf(g); // exp(-j*2*pi*5/6)
    } else {
        g1 =  conjf(g); // exp( j*2*pi*1/6)
        g2 =       -g;  // exp( j*2*pi*2/6)
        g4 = -conjf(g); // exp( j*2*pi*4/6)
        g5 =        g;  // exp( j*2*pi*5/6)
    }

    y[1] = x[0] + x[1]*g1 + x[2]*g2 - x[3] + x[4]*g4 + x[5]*g5;
    y[2] = x[0] + x[1]*g2 + x[2]*g4 + x[3] + x[4]*g2 + x[5]*g4;
    y[3] = x[0] - x[1]    + x[2]    - x[3] + x[4]    - x[5];
    y[4] = x[0] + x[1]*g4 + x[2]*g2 + x[3] + x[4]*g4 + x[5]*g2;
    y[5] = x[0] + x[1]*g5 + x[2]*g4 - x[3] + x[4]*g2 + x[5]*g1;
}

//
void FFT(_execute_dft_7)(FFT(plan) _q)
{
    TC * x = _q->x;
    TC * y = _q->y;

    // DC value is sum of inputs
    y[0] = x[0] + x[1] + x[2] + x[3] + x[4] + x[5] + x[6];

    // initialize twiddle factors
    TC g1 =  0.623489801858734 - 0.781831482468030 * _Complex_I; // exp(-j*2*pi*1/7)
    TC g2 = -0.222520933956314 - 0.974927912181824 * _Complex_I; // exp(-j*2*pi*2/7)
    TC g3 = -0.900968867902419 - 0.433883739117558 * _Complex_I; // exp(-j*2*pi*3/7)

    if (_q->direction == FFT_FORWARD) {
    } else {
        g1 = conjf(g1); // exp(+j*2*pi*1/7)
        g2 = conjf(g2); // exp(+j*2*pi*2/7)
        g3 = conjf(g3); // exp(+j*2*pi*3/7)
    }

    TC g4 = conjf(g3);
    TC g5 = conjf(g2);
    TC g6 = conjf(g1);

    y[1] = x[0] + x[1]*g1 + x[2]*g2 + x[3]*g3 + x[4]*g4 + x[5]*g5 + x[6]*g6;
    y[2] = x[0] + x[1]*g2 + x[2]*g4 + x[3]*g6 + x[4]*g1 + x[5]*g3 + x[6]*g5;
    y[3] = x[0] + x[1]*g3 + x[2]*g6 + x[3]*g2 + x[4]*g5 + x[5]*g1 + x[6]*g4;
    y[4] = x[0] + x[1]*g4 + x[2]*g1 + x[3]*g5 + x[4]*g2 + x[5]*g6 + x[6]*g3;
    y[5] = x[0] + x[1]*g5 + x[2]*g3 + x[3]*g1 + x[4]*g6 + x[5]*g4 + x[6]*g2;
    y[6] = x[0] + x[1]*g6 + x[2]*g5 + x[3]*g4 + x[4]*g3 + x[5]*g2 + x[6]*g1;
}

//
void FFT(_execute_dft_8)(FFT(plan) _q)
{
    TC yp;
    TC * x = _q->x;
    TC * y = _q->y;

    // fft or ifft?
    int fft = _q->direction == FFT_FORWARD ? 1 : 0;

    // index reversal
    y[0] = x[0];
    y[1] = x[4];
    y[2] = x[2];
    y[3] = x[6];
    y[4] = x[1];
    y[5] = x[5];
    y[6] = x[3];
    y[7] = x[7];

    // i=0
    yp = y[1];  y[1] = y[0]-yp;     y[0] += yp;
    yp = y[3];  y[3] = y[2]-yp;     y[2] += yp;
    yp = y[5];  y[5] = y[4]-yp;     y[4] += yp;
    yp = y[7];  y[7] = y[6]-yp;     y[6] += yp;


    // i=1
    yp = y[2];  y[2] = y[0]-yp;     y[0] += yp;
    yp = y[6];  y[6] = y[4]-yp;     y[4] += yp;

    if (fft) yp =  cimagf(y[3]) - crealf(y[3])*_Complex_I;
    else     yp = -cimagf(y[3]) + crealf(y[3])*_Complex_I;
    y[3] = y[1]-yp;
    y[1] += yp;

    if (fft) yp =  cimagf(y[7]) - crealf(y[7])*_Complex_I;
    else     yp = -cimagf(y[7]) + crealf(y[7])*_Complex_I;
    y[7] = y[5]-yp;
    y[5] += yp;


    // i=2
    yp = y[4];  y[4] = y[0]-yp;     y[0] += yp;

    if (fft) yp = y[5]*(M_SQRT1_2 - M_SQRT1_2*_Complex_I);
    else     yp = y[5]*(M_SQRT1_2 + M_SQRT1_2*_Complex_I);
    y[5] = y[1]-yp;
    y[1] += yp;

    if (fft) yp =  cimagf(y[6]) - crealf(y[6])*_Complex_I;
    else     yp = -cimagf(y[6]) + crealf(y[6])*_Complex_I;
    y[6] = y[2]-yp;
    y[2] += yp;

    if (fft) yp = y[7]*(-M_SQRT1_2 - M_SQRT1_2*_Complex_I);
    else     yp = y[7]*(-M_SQRT1_2 + M_SQRT1_2*_Complex_I);
    y[7] = y[3]-yp;
    y[3] += yp;
}

