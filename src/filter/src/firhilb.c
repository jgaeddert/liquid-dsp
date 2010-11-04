/*
 * Copyright (c) 2007, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2009, 2010 Virginia Polytechnic Institute &
 *                                State University
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
// firhilb.c
//
// finite impulse response (FIR) Hilbert transform
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// defined:
//  FIRHILB()       name-mangling macro
//  T               coefficients type
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

struct FIRHILB(_s) {
    T * h;                  // filter coefficients
    unsigned int h_len;     // length of filter
    float As;               // filter stop-band attenuation [dB]

    unsigned int m;         // primitive filter length (filter semi-
                            // length), h_len = 4*m+1

    // quadrature filter components
    T * hq;                 // quadrature filter coefficients
    unsigned int hq_len;    // quadrature filter length
    WINDOW() wq;            // quadrature filter window

    // in-phase 'filter' (delay line)
    T * wi;                 // window (buffer)
    unsigned int wi_index;  // index
};

// create firhilb object
//  _h_len  :   filter length
//  _As     :   stop-band attenuation [dB]
FIRHILB() FIRHILB(_create)(unsigned int _h_len,
                           float _As)
{
    // validate firhilb inputs
    if (_h_len < 1) {
        fprintf(stderr,"error(), firhilb_create(), filter length must be greater than 0\n");
        exit(1);
    }

    FIRHILB() f = (FIRHILB()) malloc(sizeof(struct FIRHILB(_s)));
    f->h_len = _h_len;
    f->As = fabsf(_As);

    // change filter length as necessary
    // h_len = 2*(2*m) + 1
    f->m = (_h_len-1)/4;
    if (f->m < 2)
        f->m = 2;

    f->h_len = 4*(f->m) + 1;
    f->h = (T *) malloc((f->h_len)*sizeof(T));

    f->hq_len = 2*(f->m);
    f->hq = (T *) malloc((f->hq_len)*sizeof(T));

    // compute filter coefficients, alternating sign
    unsigned int i;
    float t, h1, h2, s;
    float beta = kaiser_beta_slsl(f->As);
    for (i=0; i<f->h_len; i++) {
        t = (float)i - (float)(f->h_len-1)/2.0f;
        h1 = sincf(t/2.0f);
        h2 = kaiser(i,f->h_len,beta,0);
        s  = sinf(M_PI*t/2);
        f->h[i] = s*h1*h2;
    }

    // resample, reverse direction
    unsigned int j=0;
    for (i=1; i<f->h_len; i+=2)
        f->hq[j++] = f->h[f->h_len - i - 1];

    f->wq = WINDOW(_create)(2*(f->m));
    WINDOW(_clear)(f->wq);

    f->wi = (float*)malloc((f->m)*sizeof(float));
    for (i=0; i<f->m; i++)
        f->wi[i] = 0;
    f->wi_index = 0;

    return f;
}

// destroy firhilb object
void FIRHILB(_destroy)(FIRHILB() _f)
{
    WINDOW(_destroy)(_f->wq);
    free(_f->wi);
    free(_f->h);
    free(_f->hq);
    free(_f);
}

// print firhilb object internals
void FIRHILB(_print)(FIRHILB() _f)
{
    printf("fir hilbert transform: [%u]\n", _f->h_len);
    unsigned int i;
    for (i=0; i<_f->h_len; i++) {
        printf("  h(%4u) = %8.4f;\n", i+1, _f->h[i]);
    }
    printf("---\n");
    for (i=0; i<_f->hq_len; i++) {
        printf("  hq(%4u) = %8.4f;\n", i+1, _f->hq[i]);
    }
}

// clear firhilb object buffers
void FIRHILB(_clear)(FIRHILB() _f)
{
    WINDOW(_clear)(_f->wq);
    unsigned int i;
    for (i=0; i<_f->m; i++)
        _f->wi[i] = 0;
    _f->wi_index = 0;
}

// execute Hilbert transform decimator (real to complex)
//  _f      :   firhilb object
//  _x      :   real-valued input array [size: 2 x 1]
//  _y      :   complex-valued output sample
void FIRHILB(_decim_execute)(FIRHILB() _f,
                             T * _x,
                             T complex *_y)
{
    T * r;
    T yi, yq;

    // compute quadrature component (filter branch)
    WINDOW(_push)(_f->wq, _x[0]);
    WINDOW(_read)(_f->wq, &r);
    // TODO yq = DOTPROD(_execute)(_f->dpq, r);
    DOTPROD(_run)(_f->hq, r, _f->hq_len, &yq);

    // compute in-phase component (delay branch)
    yi = _f->wi[_f->wi_index];
    _f->wi[_f->wi_index] = _x[1];
    _f->wi_index = (_f->wi_index+1) % (_f->m);

    // set return value
    *_y = yi + _Complex_I * yq;
}

// execute Hilbert transform interpolator (complex to real)
//  _f      :   firhilb object
//  _y      :   complex-valued input sample
//  _x      :   real-valued output array [size: 2 x 1]
void FIRHILB(_interp_execute)(FIRHILB() _f,
                              T complex _x,
                              T *_y)
{
    T * r;  // read pointer

    // TODO macro for crealf, cimagf?
    
    // compute first branch (delay)
    _y[0] = _f->wi[_f->wi_index];
    _f->wi[_f->wi_index] = cimagf(_x);
    _f->wi_index = (_f->wi_index+1) % (_f->m);

    // compute second branch (filter)
    WINDOW(_push)(_f->wq, crealf(_x));
    WINDOW(_read)(_f->wq, &r);
    //yq = DOTPROD(_execute)(_f->dpq, r);
    DOTPROD(_run)(_f->hq, r, _f->hq_len, &_y[1]);

}

