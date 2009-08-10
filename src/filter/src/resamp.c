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

// defined:
//  TO          output data type
//  TC          coefficient data type
//  TI          input data type
//  RESAMP()    name-mangling macro
//  FIRPFB()    firpfb macro

struct RESAMP(_s) {
    TC * h;
    unsigned int h_len;

    float r;        // rate
    float tau;      // accumulated timing phase (0 <= tau <= 1)
    unsigned int b; // filterbank index

    unsigned int npfb;
    FIRPFB() f;

    fir_prototype p;
};

RESAMP() RESAMP(_create)(float _r,
                         unsigned int _h_len,
                         float _slsl,
                         unsigned int _npfb)
{
    // validate input
    if (_r > 2.0f) {
        printf("error: input rate too large...\n");
        exit(1);
    }

    RESAMP() q = (RESAMP()) malloc(sizeof(struct RESAMP(_s)));
    q->r = _r;

    // design filter
    q->h_len = _h_len;
    q->npfb  = _npfb;
    unsigned int n = _h_len*_npfb+1;
    float hf[n];
    TC h[n];
    fir_kaiser_window(n,_r,_slsl,0.0f,hf);
    unsigned int i;
    for (i=0; i<n; i++)
        h[i] = hf[i];
    q->f = FIRPFB(_create)(_npfb,h,n-1);

    q->tau = 0.0f;
    q->b   = 0;

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
}

void RESAMP(_execute)(RESAMP() _q,
                      TI _x,
                      TO * _y,
                      unsigned int *_num_written)
{
    FIRPFB(_push)(_q->f, _x);
    FIRPFB(_execute)(_q->f, _q->b, &_y[0]);
    *_num_written = 1;

    printf("tau : %12.8f, b : %3u\n", _q->tau, _q->b);
    _q->tau += _q->r - 1.0f;    // assumes r > 1.0

    _q->b = (unsigned int)roundf(_q->tau * _q->npfb);

    if (_q->b >= _q->npfb) {
        printf("extending...\n");
        _q->tau -= 1.0f;
        _q->b   -= _q->npfb;
        FIRPFB(_push)(_q->f, _x);

        FIRPFB(_execute)(_q->f, _q->b, &_y[1]);
        *_num_written++;

        printf("tau : %12.8f, b : %3u\n", _q->tau, _q->b);
        _q->tau += _q->r - 1.0f;    // assumes r > 1.0

        _q->b = (unsigned int)roundf(_q->tau * _q->npfb);
    }
}
