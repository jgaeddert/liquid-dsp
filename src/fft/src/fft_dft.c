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
    q->data.dft.dotprod = NULL;

    // check size, use specific codelet for small DFTs
    if      (q->nfft == 2) q->execute = FFT(_execute_dft_2);
    else if (q->nfft == 3) q->execute = FFT(_execute_dft_3);
#if !LIQUID_FPM
    else if (q->nfft == 4) q->execute = FFT(_execute_dft_4);
    else if (q->nfft == 5) q->execute = FFT(_execute_dft_5);
    else if (q->nfft == 6) q->execute = FFT(_execute_dft_6);
    else if (q->nfft == 7) q->execute = FFT(_execute_dft_7);
    else if (q->nfft == 8) q->execute = FFT(_execute_dft_8);
#endif
    else {
        q->execute = FFT(_execute_dft);

        // initialize twiddle factors
        unsigned int i;
        unsigned int k;
        q->data.dft.twiddle = (TC *) malloc(q->nfft * sizeof(TC));
        T d = (q->direction == FFT_FORWARD) ? -1.0 : 1.0;
        for (i=0; i<q->nfft; i++) {
            float complex t = cexpf(_Complex_I*d*2*M_PI*(T)i / (T)(q->nfft));
#if LIQUID_FPM
            q->data.dft.twiddle[i] = CQ(_float_to_fixed)(t);
#else
            q->data.dft.twiddle[i] = t;
#endif
        }

        // create dotprod objects
        q->data.dft.dotprod = (DOTPROD()*) malloc(q->nfft * sizeof(DOTPROD()));
        
        // create dotprod objects
        // twiddles: exp(-j*2*pi*W), W=
        //  0   0   0   0   0...
        //  0   1   2   3   4...
        //  0   2   4   6   8...
        //  0   3   6   9   12...
        //  ...
        // Note that first row/column is zero, no multiplication necessary.
        // Create dotprod for first row anyway because it's still faster...
        TC * dpvect = (TC *) malloc(q->nfft * sizeof(TC));
        for (i=0; i<q->nfft; i++) {
            // initialize twiddle factors
            // NOTE: no need to compute first twiddle because exp(-j*2*pi*0) = 1
            for (k=1; k<q->nfft; k++)
                dpvect[k-1] = q->data.dft.twiddle[(i*k)%(q->nfft)];

            // create dotprod object
            q->data.dft.dotprod[i] = DOTPROD(_create)(dpvect, q->nfft-1);
        }
        free(dpvect);
    }

    return q;
}

// destroy FFT plan
void FFT(_destroy_plan_dft)(FFT(plan) _q)
{
    // free twiddle factors
    if (_q->data.dft.twiddle != NULL)
        free(_q->data.dft.twiddle);

    // free dotprod objects
    if (_q->data.dft.dotprod != NULL) {
        unsigned int i;
        for (i=0; i<_q->nfft; i++)
            DOTPROD(_destroy)(_q->data.dft.dotprod[i]);

        // free dotprod array
        free(_q->data.dft.dotprod);
    }

    // free main object memory
    free(_q);
}

#if 0
// fixed-point DFT (slow but functionally correct)
void FFT(_execute_dft)(FFT(plan) _q)
{
    unsigned int i;
    unsigned int k;
    unsigned int nfft = _q->nfft;

    // DC value is sum of input
    Q(_at) ti = _q->x[0].real;
    Q(_at) tq = _q->x[0].imag;
    for (i=1; i<nfft; i++) {
        ti += _q->x[i].real;
        tq += _q->x[i].imag;
    }
    if (_q->direction == FFT_FORWARD) {
        _q->y[0].real = ti;
        _q->y[0].imag = tq;
    } else {
        _q->y[0].real = ti / (int)nfft;
        _q->y[0].imag = tq / (int)nfft;
    }
    
    // compute remaining DFT values
    for (i=1; i<nfft; i++) {
        ti = _q->x[0].real;
        tq = _q->x[0].imag;
        for (k=1; k<nfft; k++) {
            TC t0 = CQ(_mul)(_q->x[k],  _q->data.dft.twiddle[(i*k)%_q->nfft]);

            ti += t0.real;
            tq += t0.imag;
        }
        if (_q->direction == FFT_FORWARD) {
            _q->y[i].real = ti;
            _q->y[i].imag = tq;
        } else {
            _q->y[i].real = ti / (int)nfft;
            _q->y[i].imag = tq / (int)nfft;
        }
    }
}
#endif

// floating-point DFT (slow but functionally correct)
void FFT(_execute_dft)(FFT(plan) _q)
{
    unsigned int i;
    unsigned int nfft = _q->nfft;

#if 0
    // DC value is sum of input
    _q->y[0] = _q->x[0];
    for (i=1; i<nfft; i++) {
        _q->y[0] += _q->x[i];
    }
    
    // compute remaining DFT values
    unsigned int k;
    for (i=1; i<nfft; i++) {
        _q->y[i] = _q->x[0];
        for (k=1; k<nfft; k++) {
            _q->y[i] += _q->x[k] * _q->data.dft.twiddle[(i*k)%_q->nfft];
        }
    }
#else
    // use vector dot products
    // NOTE: no need to compute first multiplication because exp(-j*2*pi*0) = 1
    for (i=0; i<nfft; i++) {
        DOTPROD(_execute)(_q->data.dft.dotprod[i], &_q->x[1], &_q->y[i]);
#if LIQUID_FPM
        _q->y[i].real += _q->x[0].real;
        _q->y[i].imag += _q->x[0].imag;

        if (_q->direction != FFT_FORWARD) {
            _q->y[i].real /= (int)nfft;
            _q->y[i].imag /= (int)nfft;
        }
#else
        _q->y[i] += _q->x[0];
#endif
    }
#endif
}


// 
// codelets for small DFTs
//

// 
void FFT(_execute_dft_2)(FFT(plan) _q)
{
#if LIQUID_FPM
    _q->y[0].real = _q->x[0].real + _q->x[1].real;
    _q->y[0].imag = _q->x[0].imag + _q->x[1].imag;

    _q->y[1].real = _q->x[0].real - _q->x[1].real;
    _q->y[1].imag = _q->x[0].imag - _q->x[1].imag;
    
    if (_q->direction == FFT_REVERSE) {
        _q->y[0].real >>= 1;
        _q->y[0].imag >>= 1;
        _q->y[1].real >>= 1;
        _q->y[1].imag >>= 1;
    }
#else
    _q->y[0] = _q->x[0] + _q->x[1];
    _q->y[1] = _q->x[0] - _q->x[1];
#endif
}

//
void FFT(_execute_dft_3)(FFT(plan) _q)
{
#if LIQUID_FPM
    // NOTE: not as fast as other method, but perhaps useful for
    // fixed-point algorithm
    //  x = a + jb
    //  y = c + jd
    // We want to compute both x*y and x*conj(y) with as few
    // multiplications as possible. If we define
    //  k1 = a*(c+d);
    //  k2 = d*(a+b);
    //  k3 = c*(b-a);
    //  k4 = b*(c+d);
    // then
    //  x*y       = (k1-k2) + j(k1+k3)
    //  x*conj(y) = (k4-k3) + j(k4-k2)
    //
    // NOTE: fixed-point values:
    //    1.36602545 > 0x02bb
    //    0.86602539 > 0x01bb
    T a,  b; // c=real(g)=-0.5, d=imag(g)=-sqrt(3)/2
    T k1, k2, k3, k4;

    // compute both _q->x[1]*g and _q->x[1]*conj(g) with only 4 real multiplications
    a = _q->x[1].real;
    b = _q->x[1].imag;
    //k1 = a*(-0.5f + -0.866025403784439f);
    //k1 = -1.36602540378444f*a;
    k1 = Q(_mul)( a, Q(_float_to_fixed)(-1.36602540378444f) );
    //k2 = -0.866025403784439f*(    a + b);
    k2 = Q(_mul)( a+b, Q(_float_to_fixed)(-0.866025403784439f) );
    //k3 = -0.5f*(    b - a);
    k3 = -((b - a)>>1);
    //k4 =                   b*(-0.5f + -0.866025403784439f);
    //k4 = -1.36602540378444f*b;
    k4 = Q(_mul)( b, Q(_float_to_fixed)(-1.36602540378444f) );

    TC ta1 = { (k1-k2), (k1+k3) };   // 
    TC tb1 = { (k4-k3), (k4-k2) };   // 
    
    // compute both _q->x[2]*g and _q->x[2]*conj(g) with only 4 real multiplications
    a = _q->x[2].real;
    b = _q->x[2].imag;
#if 0
    //k1 = a*(-0.5f + -0.866025403784439f);
    k1 = -1.36602540378444f*a;
    k2 = -0.866025403784439f*(    a + b);
    k3 =               -0.5f*(    b - a);
    //k4 =                   b*(-0.5f + -0.866025403784439f);
    k4 = -1.36602540378444f*b;

    TC ta2 = (k1-k2) + _Complex_I*(k1+k3);   // 
    TC tb2 = (k4-k3) + _Complex_I*(k4-k2);   // 
#else
    //k1 = a*(-0.5f + -0.866025403784439f);
    //k1 = -1.36602540378444f*a;
    k1 = Q(_mul)( a, Q(_float_to_fixed)(-1.36602540378444f) );
    //k2 = -0.866025403784439f*(    a + b);
    k2 = Q(_mul)( a+b, Q(_float_to_fixed)(-0.866025403784439f) );
    //k3 = -0.5f*(    b - a);
    k3 = -((b - a)>>1);
    //k4 =                   b*(-0.5f + -0.866025403784439f);
    //k4 = -1.36602540378444f*b;
    k4 = Q(_mul)( b, Q(_float_to_fixed)(-1.36602540378444f) );

    TC ta2 = { (k1-k2), (k1+k3) };   // 
    TC tb2 = { (k4-k3), (k4-k2) };   // 
#endif
    
    // set return values
    _q->y[0].real = _q->x[0].real + _q->x[1].real + _q->x[2].real;
    _q->y[0].imag = _q->x[0].imag + _q->x[1].imag + _q->x[2].imag;
    if (_q->direction == FFT_FORWARD) {
        _q->y[1].real = _q->x[0].real + ta1.real + tb2.real;
        _q->y[1].imag = _q->x[0].imag + ta1.imag + tb2.imag;

        _q->y[2].real = _q->x[0].real + tb1.real + ta2.real;
        _q->y[2].imag = _q->x[0].imag + tb1.imag + ta2.imag;
    } else {
        _q->y[1].real = _q->x[0].real + tb1.real + ta2.real;
        _q->y[1].imag = _q->x[0].imag + tb1.imag + ta2.imag;

        _q->y[2].real = _q->x[0].real + ta1.real + tb2.real;
        _q->y[2].imag = _q->x[0].imag + ta1.imag + tb2.imag;

        _q->y[0].real /= 3;
        _q->y[0].imag /= 3;

        _q->y[1].real /= 3;
        _q->y[1].imag /= 3;

        _q->y[2].real /= 3;
        _q->y[2].imag /= 3;
    }
#else
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
#endif
}

#if !LIQUID_FPM
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
#endif
