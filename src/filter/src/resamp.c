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
// Arbitrary resampler
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DEBUG_RESAMP_PRINT  1

// defined:
//  TO          output data type
//  TC          coefficient data type
//  TI          input data type
//  RESAMP()    name-mangling macro
//  FIRPFB()    firpfb macro

struct RESAMP(_s) {
    TC * h;
    unsigned int h_len;
    float slsl;
    float fc;

    float r;        // rate
    float tau;      // accumulated timing phase (0 <= tau <= 1)
    float bf;       // soft filterbank index
    int b;          // filterbank index
    float g;        // filter compensation gain
    float del;      // fractional delay step

    unsigned int npfb;
    FIRPFB() f;

    fir_prototype p;
};

RESAMP() RESAMP(_create)(float _r,
                         unsigned int _h_len,
                         float _fc,
                         float _slsl,
                         unsigned int _npfb)
{
    // TODO: validate input

    RESAMP() q = (RESAMP()) malloc(sizeof(struct RESAMP(_s)));
    q->r     = _r;
    q->slsl  = _slsl;
    q->fc    = _fc;
    q->h_len = _h_len;
    q->npfb  = _npfb;

    // design filter
    unsigned int n = 2*_h_len*_npfb+1;
    float hf[n];
    TC h[n];
    fir_kaiser_window(n,q->fc/((float)(q->npfb)),q->slsl,0.0f,hf);
    unsigned int i;
    for (i=0; i<n; i++)
        h[i] = hf[i];
    q->f = FIRPFB(_create)(_npfb,h,n-1);

    // set gain
    q->g = 0.;
    for (i=0; i<n; i++)
        q->g += h[i];
    q->g = (q->npfb)/(q->g);

    //for (i=0; i<n; i++)
    //    PRINTVAL_TC(stdout,"h",i,h[i]);
    //exit(0);

    q->tau = 0.0f;
    q->bf  = 0.0f;
    q->b   = 0;
    q->del = 1.0f / q->r;

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
    _q->tau = 0.0f;
    _q->bf  = 0.0f;
    _q->b   = 0;
}

void RESAMP(_setrate)(RESAMP() _q, float _rate)
{
    // TODO : validate rate, validate this method
    _q->r = _rate;
    _q->del = 1.0f / _q->r;
}

void RESAMP(_execute)(RESAMP() _q,
                      TI _x,
                      TO * _y,
                      unsigned int *_num_written)
{
    FIRPFB(_push)(_q->f, _x);
    unsigned int n=0;
    //while (_q->bf < (float)(_q->npfb)) {
    while (_q->tau < 1.0f) {
        _q->bf = _q->tau * (float)(_q->npfb);
        //_q->b  = _q->npfb - (int)floorf(_q->bf) - 1;
        _q->b  = (int)floorf(_q->bf);
#if DEBUG_RESAMP_PRINT
        printf("  [%2u] : tau : %12.8f, b : %4u (%12.8f)\n", n, _q->tau, _q->b, _q->bf);
#endif
        FIRPFB(_execute)(_q->f, _q->b, &_y[n]);
        _y[n] *= _q->g;

        _q->tau += _q->del;
        n++;
    }

    _q->tau -= 1.0f;
    _q->bf  -= (float)(_q->npfb);
    _q->b   -= _q->npfb;
    *_num_written = n;
}

