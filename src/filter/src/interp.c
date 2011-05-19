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
// Interpolator
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// defined:
//  INTERP()    name-mangling macro
//  TO          output data type
//  TC          coefficient data type
//  TI          input data type
//  WINDOW()    window macro
//  DOTPROD()   dotprod macro
//  PRINTVAL()  print macro

struct INTERP(_s) {
    TC * h;
    unsigned int h_len;
    unsigned int M;

    WINDOW() w;
    //DOTPROD() dp;

#if 0
    fir_prototype p;    // prototype object
#endif
};

// interp_xxxt_create()
//
// create interpolator
//  _M      :   interpolation factor
//  _h      :   filter coefficients array [size: 1 x _h_len]
//  _h_len  :   filter length
INTERP() INTERP(_create)(unsigned int _M,
                         TC *_h,
                         unsigned int _h_len)
{
    INTERP() q = (INTERP()) malloc(sizeof(struct INTERP(_s)));
    q->h_len = _h_len;
    q->h = (TC*) malloc((q->h_len)*sizeof(TC));
    // load filter in reverse order
    unsigned int i;
    for (i=0; i<q->h_len; i++)
        q->h[i] = _h[_h_len-i-1];

    q->M = _M;

    q->w = WINDOW(_create)(q->h_len);
    WINDOW(_clear)(q->w);

    return q;
}

// create interpolator from prototype
//  _M      :   interpolation factor
//  _m      :   symbol delay
//  _As     :   stop-band attenuation [dB]
INTERP() INTERP(_create_prototype)(unsigned int _M,
                                   unsigned int _m,
                                   float _As)
{
    // validate input
    if (_M < 2) {
        fprintf(stderr,"error: interp_xxxt_create_prototype(), interp factor must be greater than 1\n");
        exit(1);
    } else if (_m == 0) {
        fprintf(stderr,"error: interp_xxxt_create_prototype(), filter delay must be greater than 0\n");
        exit(1);
    } else if (_As < 0.0f) {
        fprintf(stderr,"error: interp_xxxt_create_prototype(), stop-band attenuation must be positive\n");
        exit(1);
    }

    INTERP() q = (INTERP()) malloc(sizeof(struct INTERP(_s)));
    q->h_len = 2*_M*_m + 1;
    q->M = _M;
    q->h = (TC*) malloc((q->h_len)*sizeof(TC));

    // create filter using Kaiser window design
    float hf[q->h_len];
    float fc = 0.5f / (float) (q->M);
    firdes_kaiser_window(q->h_len, fc, _As, 0.0f, hf);

    // load filter in reverse order
    unsigned int i;
    for (i=0; i<q->h_len; i++)
        q->h[i] = hf[q->h_len-i-1];

    q->w = WINDOW(_create)(q->h_len);
    WINDOW(_clear)(q->w);

    return q;
}

// interp_xxxt_create_rnyquist()
//
// create root raised-cosine interpolator
//  _k      :   samples/symbol _k > 1
//  _m      :   filter delay (symbols), _m > 0
//  _beta   :   excess bandwidth factor, 0 < _beta < 1
//  _dt     :   fractional sample delay, 0 <= _dt < 1
INTERP() INTERP(_create_rnyquist)(int _type,
                                  unsigned int _k,
                                  unsigned int _m,
                                  float _beta,
                                  float _dt)
{
    // generate rrc filter
    unsigned int h_len = 2*_k*_m + 1;
    float h[h_len];
    design_rnyquist_filter(_type,_k,_m,_beta,_dt,h);

    // copy coefficients to type-specific array (e.g. float complex)
    unsigned int i;
    TC ht[h_len];
    for (i=0; i<h_len; i++)
        ht[i] = h[i];

    return INTERP(_create)(_k, ht, h_len);
}

// destroy interpolator object
void INTERP(_destroy)(INTERP() _q)
{
    WINDOW(_destroy)(_q->w);
    free(_q->h);
    free(_q);
}

// print interpolator state
void INTERP(_print)(INTERP() _q)
{
    printf("interp() [%u] :\n", _q->M);
    printf("  window:\n");
    WINDOW(_print)(_q->w);
}

// clear internal state
void INTERP(_clear)(INTERP() _q)
{
    WINDOW(_clear)(_q->w);
}

// interp_xxxt_execute()
//
// execute interpolator
//  _q      :   interpolator object
//  _x      :   input sample
//  _y      :   output buffer [size: 1 x _M]
// TODO : only compute necessary multiplications in interp_xxt_execute
void INTERP(_execute)(INTERP() _q,
                      TI _x,
                      TO *_y)
{
    TI * r; // read pointer

    unsigned int i;
    for (i=0; i<_q->M; i++) {
        if (i == 0)
            WINDOW(_push)(_q->w,_x);
        else
            WINDOW(_push)(_q->w,0);

        WINDOW(_read)(_q->w,&r);
        DOTPROD(_run4)(_q->h, r, _q->h_len, &_y[i]);
    }
}

