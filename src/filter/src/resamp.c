/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
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
// Arbitrary resampler
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

// use fixed-point phase vs. floating-point phase
#define RESAMP_USE_FIXED_POINT_PHASE    1

struct RESAMP(_s) {
    TC * h;
    unsigned int h_len;
    float As;       // stop-band attenuation
    float fc;       // filter cutoff

    float r;        // rate
    int b;          // filterbank index
    float del;      // fractional delay step

#if RESAMP_USE_FIXED_POINT_PHASE
    // fixed-point phase
    unsigned int theta;             // sampling phase
    unsigned int d_theta;           // phase differential

    unsigned int num_bits_phase;    // number of bits in phase
    unsigned int max_phase;         // maximum phase value
    unsigned int num_bits_npfb;     // number of bits in npfb
    unsigned int num_shift_bits;    // number of bits to shift to compute b
#else
    // floating-point phase
    float tau;      // accumulated timing phase (0 <= tau <= 1)
    float bf;       // soft filterbank index
#endif

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

#if RESAMP_USE_FIXED_POINT_PHASE
    q->num_bits_npfb = liquid_nextpow2(_npfb);
    q->npfb = 1<<q->num_bits_npfb;
    q->num_bits_phase = 20;
    q->max_phase = 1 << q->num_bits_phase;
    q->num_shift_bits = q->num_bits_phase - q->num_bits_npfb;
    q->theta = 0;
    q->d_theta = (unsigned int)( q->max_phase * q->del );
#else
    q->npfb = _npfb;
    q->tau = 0.0f;
    q->bf  = 0.0f;
#endif

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
    FIRPFB(_clear)(_q->f);
    _q->b   = 0;

#if RESAMP_USE_FIXED_POINT_PHASE
    _q->theta = 0;
#else
    _q->tau = 0.0f;
    _q->bf  = 0.0f;
#endif
}

void RESAMP(_setrate)(RESAMP() _q, float _rate)
{
    // TODO : validate rate, validate this method
    _q->r = _rate;
    _q->del = 1.0f / _q->r;

#if RESAMP_USE_FIXED_POINT_PHASE
    _q->d_theta = (unsigned int)( _q->max_phase * _q->del );
#endif
}

void RESAMP(_execute)(RESAMP() _q,
                      TI _x,
                      TO * _y,
                      unsigned int *_num_written)
{
    FIRPFB(_push)(_q->f, _x);
    unsigned int n=0;
    
    //while (_q->tau < 1.0f) {
#if RESAMP_USE_FIXED_POINT_PHASE
    while (_q->theta < _q->max_phase) {
#else
    while (_q->b < _q->npfb) {
#endif

#if DEBUG_RESAMP_PRINT
#  if RESAMP_USE_FIXED_POINT_PHASE
        printf("  [%2u] : theta = %6u, b : %6u\n", n, _q->theta, _q->b);
#  else
        printf("  [%2u] : tau : %12.8f, b : %4u (%12.8f)\n", n, _q->tau, _q->b, _q->bf);
#  endif
#endif
        FIRPFB(_execute)(_q->f, _q->b, &_y[n]);

#if RESAMP_USE_FIXED_POINT_PHASE
        _q->theta += _q->d_theta;
        _q->b = _q->theta >> _q->num_shift_bits;
#else
        _q->tau += _q->del;
        _q->bf = _q->tau * (float)(_q->npfb);
        _q->b  = (int)roundf(_q->bf);
#endif
        n++;
    }

#if RESAMP_USE_FIXED_POINT_PHASE
    _q->theta -= _q->max_phase;
    _q->b = _q->theta >> _q->num_shift_bits;
#else
    _q->tau -= 1.0f;
    _q->bf  -= (float)(_q->npfb);
    _q->b   -= _q->npfb;
#endif
    *_num_written = n;
}

