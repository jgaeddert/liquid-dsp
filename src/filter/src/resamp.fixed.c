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
// Arbitrary resampler (fixed-point phase verions)
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DEBUG_RESAMP_PRINT  0

// defined:
//  TO          output data type
//  TC          coefficient data type
//  TI          input data type
//  RESAMP()    name-mangling macro
//  FIRPFB()    firpfb macro

struct RESAMP(_s) {
    TC * h;
    unsigned int h_len;
    float As;       // stop-band attenuation
    float fc;       // filter cutoff

    float r;        // rate
    int b;          // filterbank index
    float del;      // fractional delay step

    // fixed-point phase
    unsigned int theta;             // sampling phase
    unsigned int d_theta;           // phase differential

    unsigned int num_bits_phase;    // number of bits in phase
    unsigned int max_phase;         // maximum phase value
    unsigned int num_bits_npfb;     // number of bits in npfb
    unsigned int num_shift_bits;    // number of bits to shift to compute b

    unsigned int npfb;
    FIRPFB() f;

#if 0
    fir_prototype p;    // prototype object
#endif
};

RESAMP() RESAMP(_create)(float _r,
                         unsigned int _h_len,
                         float _fc,
                         float _As,
                         unsigned int _npfb)
{
    // validate input
    if (_r <= 0) {
        fprintf(stderr,"error: resamp_%s_create(), resampling rate must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    } else if (_h_len == 0) {
        fprintf(stderr,"error: resamp_%s_create(), filter length must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    } else if (_npfb == 0) {
        fprintf(stderr,"error: resamp_%s_create(), number of filter banks must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    } else if (_fc <= 0.0f || _fc >= 0.5f) {
        fprintf(stderr,"error: resamp_%s_create(), filter cutoff must be in (0,0.5)\n", EXTENSION_FULL);
        exit(1);
    } else if (_As <= 0.0f) {
        fprintf(stderr,"error: resamp_%s_create(), filter stop-band suppression must be greater than zero\n", EXTENSION_FULL);
        exit(1);
    }

    RESAMP() q = (RESAMP()) malloc(sizeof(struct RESAMP(_s)));
    q->r     = _r;
    q->As    = _As;
    q->fc    = _fc;
    q->h_len = _h_len;

    q->b   = 0;
    q->del = 1.0f / q->r;

    q->num_bits_npfb = liquid_nextpow2(_npfb);
    q->npfb = 1<<q->num_bits_npfb;
    q->num_bits_phase = 20;
    q->max_phase = 1 << q->num_bits_phase;
    q->num_shift_bits = q->num_bits_phase - q->num_bits_npfb;
    q->theta = 0;
    q->d_theta = (unsigned int)( q->max_phase * q->del );

    // design filter
    unsigned int n = 2*_h_len*q->npfb+1;
    float hf[n];
    TC h[n];
    liquid_firdes_kaiser(n,q->fc/((float)(q->npfb)),q->As,0.0f,hf);

    // normalize filter coefficients by DC gain
    unsigned int i;
    float gain=0.0f;
    for (i=0; i<n; i++)
        gain += hf[i];
    gain = (q->npfb)/(gain);

    // copy to type-specific array
    for (i=0; i<n; i++)
        h[i] = hf[i]*gain;
    q->f = FIRPFB(_create)(q->npfb,h,n-1);

    //for (i=0; i<n; i++)
    //    PRINTVAL_TC(h[i],%12.8f);
    //exit(0);

    return q;
}

void RESAMP(_destroy)(RESAMP() _q)
{
    FIRPFB(_destroy)(_q->f);
    free(_q);
}

void RESAMP(_print)(RESAMP() _q)
{
    printf("resampler [rate: %f]\n", _q->r);
    FIRPFB(_print)(_q->f);
}

void RESAMP(_reset)(RESAMP() _q)
{
    FIRPFB(_reset)(_q->f);
    _q->b   = 0;

    _q->theta = 0;
}

void RESAMP(_setrate)(RESAMP() _q, float _rate)
{
    // TODO : validate rate, validate this method
    _q->r = _rate;
    _q->del = 1.0f / _q->r;

    _q->d_theta = (unsigned int)( _q->max_phase * _q->del );
}

void RESAMP(_execute)(RESAMP() _q,
                      TI _x,
                      TO * _y,
                      unsigned int *_num_written)
{
    FIRPFB(_push)(_q->f, _x);
    unsigned int n=0;
    
    //while (_q->tau < 1.0f) {
    while (_q->theta < _q->max_phase) {

#if DEBUG_RESAMP_PRINT
        printf("  [%2u] : theta = %6u, b : %6u\n", n, _q->theta, _q->b);
#endif
        FIRPFB(_execute)(_q->f, _q->b, &_y[n]);

        _q->theta += _q->d_theta;
        _q->b = _q->theta >> _q->num_shift_bits;
        n++;
    }

    _q->theta -= _q->max_phase;
    _q->b = _q->theta >> _q->num_shift_bits;
    *_num_written = n;
}

