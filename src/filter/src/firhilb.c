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
    T complex * hc;         // filter coefficients (complex)
    unsigned int h_len;     // length of filter
    float As;               // filter stop-band attenuation [dB]

    unsigned int m;         // primitive filter length (filter semi-
                            // length), h_len = 4*m+1

    // quadrature filter components
    T * hq;                 // quadrature filter coefficients
    unsigned int hq_len;    // quadrature filter length
    WINDOW() wq;            // quadrature filter window

    // in-phase 'filter' (delay line)
#if 0
    T * wi;                 // window (buffer)
#else
    WINDOW() wi;            // window (buffer)
#endif
    unsigned int wi_index;  // index

    // regular r2c|c2r operation
    unsigned int toggle;
};

// create firhilb object
//  _m      :   filter semi-length (delay: 2*m+1)
//  _As     :   stop-band attenuation [dB]
FIRHILB() FIRHILB(_create)(unsigned int _m,
                           float _As)
{
    // validate firhilb inputs
    if (_m < 2) {
        fprintf(stderr,"error: firhilb_create(), filter semi-length (m) must be at least 2\n");
        exit(1);
    }

    FIRHILB() q = (FIRHILB()) malloc(sizeof(struct FIRHILB(_s)));
    q->m  = _m;
    q->As = fabsf(_As);

    q->h_len = 4*(q->m) + 1;
    q->h  = (T *)         malloc((q->h_len)*sizeof(T));
    q->hc = (T complex *) malloc((q->h_len)*sizeof(T complex));

    q->hq_len = 2*(q->m);
    q->hq = (T *) malloc((q->hq_len)*sizeof(T));

    // compute filter coefficients for half-band filter
    firdes_kaiser_window(q->h_len, 0.25f, q->As, 0.0f, q->h);

    // alternate sign of non-zero elements
    unsigned int i;
    for (i=0; i<q->h_len; i++) {
        float t = (float)i - (float)(q->h_len-1)/2.0f;
        //q->h[i]  *= sinf(0.5f*M_PI*t);
        q->hc[i] = q->h[i] * cexpf(_Complex_I*0.5f*M_PI*t);
        q->h[i]  = cimagf(q->hc[i]);
    }

    // resample, reverse direction
    unsigned int j=0;
    for (i=1; i<q->h_len; i+=2)
        q->hq[j++] = q->h[q->h_len - i - 1];

    q->wq = WINDOW(_create)(2*(q->m));
    WINDOW(_clear)(q->wq);

#if 0
    q->wi = (float*)malloc((q->m)*sizeof(float));
    for (i=0; i<q->m; i++)
        q->wi[i] = 0;
    q->wi_index = 0;
#else
    q->wi = WINDOW(_create)(2*(q->m));
    WINDOW(_clear)(q->wi);
#endif

    return q;
}

// destroy firhilb object
void FIRHILB(_destroy)(FIRHILB() _q)
{
    WINDOW(_destroy)(_q->wq);
#if 0
    free(_q->wi);
#else
    WINDOW(_destroy)(_q->wi);
#endif
    free(_q->h);
    free(_q->hc);
    free(_q->hq);
    free(_q);
}

// print firhilb object internals
void FIRHILB(_print)(FIRHILB() _q)
{
    printf("fir hilbert transform: [%u]\n", _q->h_len);
    unsigned int i;
    for (i=0; i<_q->h_len; i++) {
        //printf("  h(%4u) = %8.4f;\n", i+1, _q->h[i]);
        printf("  hc(%4u) = %8.4f + j*%8.4f;\n", i+1, crealf(_q->hc[i]), cimagf(_q->hc[i]));
    }
    printf("---\n");
    for (i=0; i<_q->h_len; i++) {
        printf("  h(%4u) = %8.4f;\n", i+1, _q->h[i]);
    }
    printf("---\n");
    for (i=0; i<_q->hq_len; i++) {
        printf("  hq(%4u) = %8.4f;\n", i+1, _q->hq[i]);
    }
}

// clear firhilb object buffers
void FIRHILB(_clear)(FIRHILB() _q)
{
    WINDOW(_clear)(_q->wq);
#if 0
    unsigned int i;
    for (i=0; i<_q->m; i++)
        _q->wi[i] = 0;
    _q->wi_index = 0;
#else
    WINDOW(_clear)(_q->wi);
#endif

    //
    _q->toggle = 0;
}

// execute Hilbert transform (real to complex)
//  _q      :   firhilb object
//  _x      :   real-valued input sample
//  _y      :   complex-valued output sample
void FIRHILB(_r2c_execute)(FIRHILB() _q,
                           T _x,
                           T complex * _y)
{
    //
    T * r;
    T yi, yq;

    if ( _q->toggle == 0 ) {
        // push sample into upper branch
        WINDOW(_push)(_q->wi, _x);

        // upper branch (delay)
        WINDOW(_index)(_q->wi, _q->m-1, &yi);

        // lower branch (filter)
        WINDOW(_read)(_q->wq, &r);
        DOTPROD(_run)(_q->hq, r, _q->hq_len, &yq);
    } else {
        // push sample into lower branch
        WINDOW(_push)(_q->wq, _x);

        // upper branch (delay)
        WINDOW(_index)(_q->wq, _q->m-1, &yi);

        // lower branch (filter)
        WINDOW(_read)(_q->wi, &r);
        DOTPROD(_run)(_q->hq, r, _q->hq_len, &yq);
    }

    _q->toggle = 1 - _q->toggle;

    // set return value
    *_y = yi + _Complex_I * yq;
}

// execute Hilbert transform (complex to real)
//  _q      :   firhilb object
//  _y      :   complex-valued input sample
//  _x      :   real-valued output sample
void FIRHILB(_c2r_execute)(FIRHILB() _q,
                           T complex _x,
                           T * _y)
{
    *_y = crealf(_x);
}

// execute Hilbert transform decimator (real to complex)
//  _q      :   firhilb object
//  _x      :   real-valued input array [size: 2 x 1]
//  _y      :   complex-valued output sample
void FIRHILB(_decim_execute)(FIRHILB() _q,
                             T * _x,
                             T complex *_y)
{
    T * r;
    T yi, yq;

    // compute quadrature component (filter branch)
    WINDOW(_push)(_q->wq, _x[0]);
    WINDOW(_read)(_q->wq, &r);
    // TODO yq = DOTPROD(_execute)(_q->dpq, r);
    DOTPROD(_run)(_q->hq, r, _q->hq_len, &yq);

#if 0
    // compute in-phase component (delay branch)
    yi = _q->wi[_q->wi_index];
    _q->wi[_q->wi_index] = _x[1];
    _q->wi_index = (_q->wi_index+1) % (_q->m);
#else
    WINDOW(_push)(_q->wi, _x[1]);
    WINDOW(_index)(_q->wi, _q->m-1, &yi);
#endif

    // set return value
    *_y = yi + _Complex_I * yq;
}

// execute Hilbert transform interpolator (complex to real)
//  _q      :   firhilb object
//  _y      :   complex-valued input sample
//  _x      :   real-valued output array [size: 2 x 1]
void FIRHILB(_interp_execute)(FIRHILB() _q,
                              T complex _x,
                              T *_y)
{
    T * r;  // read pointer

    // TODO macro for crealf, cimagf?
    
#if 0
    // compute first branch (delay)
    _y[0] = _q->wi[_q->wi_index];
    _q->wi[_q->wi_index] = cimagf(_x);
    _q->wi_index = (_q->wi_index+1) % (_q->m);
#else
    WINDOW(_push)(_q->wi, cimagf(_x));
    WINDOW(_index)(_q->wi, _q->m-1, &_y[0]);
#endif

    // compute second branch (filter)
    WINDOW(_push)(_q->wq, crealf(_x));
    WINDOW(_read)(_q->wq, &r);
    //yq = DOTPROD(_execute)(_q->dpq, r);
    DOTPROD(_run)(_q->hq, r, _q->hq_len, &_y[1]);

}

