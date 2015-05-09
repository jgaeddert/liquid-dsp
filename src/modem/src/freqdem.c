/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

//
// Frequency demodulator
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "liquidfpm.internal.h"
#include "liquid.internal.h"

// freqdem
struct FREQDEM(_s) {
    // common
    float kf;   // modulation index
    T     ref;  // 1/(2*pi*kf)

    TC r_prime; // previous received sample
};

// create freqdem object
//  _kf     :   modulation factor
FREQDEM() FREQDEM(_create)(float _kf)
{
    // validate input
    if (_kf <= 0.0f || _kf > 1.0) {
        fprintf(stderr,"error: freqdem_create(), modulation factor %12.4e out of range [0,1]\n", _kf);
        exit(1);
    }

    // create main object memory
    FREQDEM() q = (FREQDEM()) malloc(sizeof(struct FREQDEM(_s)));

    // set internal modulation factor
    q->kf = _kf;

    // compute reference
#if defined LIQUID_FPM
    // FIXME: thoroughly test this fixed-point reference value
    q->ref = (1<<3) / q->kf;
#else
    q->ref = 1.0f / (2*M_PI*q->kf);
#endif

    // reset modem object
    FREQDEM(_reset)(q);

    // return object
    return q;
}

// destroy modem object
void FREQDEM(_destroy)(FREQDEM() _q)
{
    // free main object memory
    free(_q);
}

// print modulation internals
void FREQDEM(_print)(FREQDEM() _q)
{
    printf("freqdem:\n");
    printf("    mod. factor :   %8.4f\n", _q->kf);
}

// reset modem object
void FREQDEM(_reset)(FREQDEM() _q)
{
    // clear complex phase term
#if defined LIQUID_FPM
    _q->r_prime = CQ(_zero);
#else
    _q->r_prime = 0;
#endif
}

// demodulate sample
//  _q      :   FM demodulator object
//  _r      :   received signal
//  _m      :   output message signal
void FREQDEM(_demodulate)(FREQDEM() _q,
                          TC        _r,
                          T *       _m)
{
#if defined LIQUID_FPM
    // compute phase difference and normalize by modulation index
    TC conj_mul = CQ(_mul)( CQ(_conj)(_q->r_prime), _r );
    //T  dphi_hat = Q(_atan2_frac)( conj_mul.imag, conj_mul.real ); // fast, but less accurate
    T  dphi_hat = CQ(_carg)( conj_mul );    // more accurate, but considerably slower
    *_m = Q(_mul)( dphi_hat, _q->ref );
    
    // super fast method, but requires _r to be normalized to 1 without
    // penalty of output scaling improperly
    // TODO: normalize _r to 1
    //*_m = Q(_mul)(_q->r_prime.real, _r.imag) -
    //      Q(_mul)(_q->r_prime.imag, _r.real);
#else
    // compute phase difference and normalize by modulation index
    *_m = cargf( conjf(_q->r_prime)*_r ) * _q->ref;
#endif

    // save previous input sample
    _q->r_prime = _r;
}

// demodulate block of samples
//  _q      :   frequency demodulator object
//  _r      :   received signal r(t) [size: _n x 1]
//  _n      :   number of input, output samples
//  _m      :   message signal m(t), [size: _n x 1]
void FREQDEM(_demodulate_block)(FREQDEM()    _q,
                                TC *         _r,
                                unsigned int _n,
                                T *          _m)
{
    // TODO: implement more efficient method
    unsigned int i;
    for (i=0; i<_n; i++)
        FREQDEM(_demodulate)(_q, _r[i], &_m[i]);
}

