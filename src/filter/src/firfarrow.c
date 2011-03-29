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
// firfarrow.c
//
// Finite impulse response Farrow filter
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define FIRFARROW_USE_DOTPROD 1

#define FIRFARROW_DEBUG 0

// defined:
//  FIRFARROW()     name-mangling macro
//  T               coefficients type
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

struct FIRFARROW(_s) {
    TC * h;             // filter coefficients
    unsigned int h_len; // filter length
    float fc;           // filter cutoff
    float As;           // stop-band attenuation [dB]
    unsigned int Q;     // polynomial order

    float mu;           // fractional sample delay
    float * P;          // polynomail coefficients matrix [ h_len x Q+1 ]
    float gamma;        // inverse of DC response (normalization factor)

#if FIRFARROW_USE_DOTPROD
    WINDOW() w;
#else
    TI * v;
    unsigned int v_index;
#endif
};

// create firfarrow object
//  _h_len      :   filter length
//  _p          :   polynomial order
//  _fc         :   filter cutoff frequency
//  _As         :   stopband attenuation [dB]
FIRFARROW() FIRFARROW(_create)(unsigned int _h_len,
                               unsigned int _p,
                               float _fc,
                               float _As)
{
    // validate input
    if (_h_len < 2) {
        fprintf(stderr,"error: firfarrow_xxxt_create(), filter length must be > 2\n");
        exit(1);
    } else if (_p < 1) {
        fprintf(stderr,"error: firfarrow_xxxt_create(), polynomial order must be at least 1\n");
        exit(1);
    } else if (_fc < 0.0f || _fc > 0.5f) {
        fprintf(stderr,"error: firfarrow_xxxt_create(), filter cutoff must be in [0,0.5]\n");
        exit(1);
    }

    FIRFARROW() f = (FIRFARROW()) malloc(sizeof(struct FIRFARROW(_s)));
    f->h_len = _h_len;  // filter length
    f->Q     = _p;      // polynomial order
    f->As    = _As;     // filter stop-band attenuation
    f->fc    = _fc;     // filter cutoff frequency

    // allocate memory for filter coefficients
    f->h = (TC *) malloc((f->h_len)*sizeof(TC));

#if FIRFARROW_USE_DOTPROD
    f->w = WINDOW(_create)(f->h_len);
#else
    f->v = malloc((f->h_len)*sizeof(TI));
#endif

    // allocate memory for polynomial matrix [ h_len x Q+1 ]
    f->P = (float*) malloc((f->h_len)*(f->Q+1)*sizeof(float));

    // clear the filter object
    FIRFARROW(_clear)(f);

    // generate polynomials
    FIRFARROW(_genpoly)(f);

    // set nominal delay of 0
    FIRFARROW(_set_delay)(f,0.0f);

    return f;
}

// destroy firfarrow object
void FIRFARROW(_destroy)(FIRFARROW() _f)
{
#if FIRFARROW_USE_DOTPROD
    WINDOW(_destroy)(_f->w);
#else
    free(_f->v);
#endif
    free(_f->h);    // free the filter coefficients array
    free(_f->P);    // free the polynomial matrix
    free(_f);
}

// clear firfarrow object
void FIRFARROW(_clear)(FIRFARROW() _f)
{
#if FIRFARROW_USE_DOTPROD
    WINDOW(_clear)(_f->w);
#else
    unsigned int i;
    for (i=0; i<_f->h_len; i++)
        _f->v[i] = 0;
    _f->v_index = 0;
#endif
}

// print firfarrow object internals
void FIRFARROW(_print)(FIRFARROW() _f)
{
    printf("firfarrow [len : %u, poly-order : %u]\n", _f->h_len, _f->Q);
    printf("polynomial coefficients:\n");

    // print coefficients
    unsigned int i, j, n=0;
    for (i=0; i<_f->h_len; i++) {
        printf("  %3u : ", i);
        for (j=0; j<_f->Q+1; j++)
            printf("%12.4e ", _f->P[n++]);
        printf("\n");
    }

    printf("filter coefficients (mu=%8.4f):\n", _f->mu);
    n = _f->h_len;
    for (i=0; i<n; i++) {
        printf("  h(%3u) = ", i+1);
        PRINTVAL_TC(_f->h[n-i-1],%12.8f);
        printf(";\n");
    }
}

// push sample into firfarrow object
//  _f      :   firfarrow object
//  _x      :   input sample
void FIRFARROW(_push)(FIRFARROW() _f,
                      TI _x)
{
#if FIRFARROW_USE_DOTPROD
    WINDOW(_push)(_f->w, _x);
#else
    _f->v[ _f->v_index ] = _x;
    (_f->v_index)++;
    _f->v_index = (_f->v_index) % (_f->h_len);
#endif
}

// set fractional delay of firfarrow object
//  _f      :   firfarrow object
//  _mu     :   fractional sample delay
void FIRFARROW(_set_delay)(FIRFARROW() _f,
                           float _mu)
{
    // validate input
    if (_mu < -1.0f || _mu > 1.0f) {
        fprintf(stderr,"warning: firfarrow_xxxt_set_delay(), delay out of range\n");
    }

    unsigned int i, n=0;
    for (i=0; i<_f->h_len; i++) {
        // compute filter tap from polynomial using negative
        // value for _mu
        _f->h[i] = POLY(_val)(_f->P+n, _f->Q, -_mu);

        // normalize filter by inverse of DC response
        _f->h[i] *= _f->gamma;

        n += _f->Q+1;

        //printf("  h[%3u] = %12.8f\n", i, _f->h[i]);
    }
}

// execute firfarrow internal dot product
//  _f      :   firfarrow object
//  _y      :   output sample pointer
void FIRFARROW(_execute)(FIRFARROW() _f,
                         TO *_y)
{
#if FIRFARROW_USE_DOTPROD
    TI *r;
    WINDOW(_read)(_f->w, &r);
    DOTPROD(_run4)(_f->h, r, _f->h_len, _y);
#else
    TO y = 0;
    unsigned int i;
    for (i=0; i<_f->h_len; i++)
        y += _f->v[ (i+_f->v_index)%(_f->h_len) ] * _f->h[i];
    *_y = y;
#endif
}

// get length of firfarrow object (number of filter taps)
unsigned int FIRFARROW(_get_length)(FIRFARROW() _f)
{
    return _f->h_len;
}

// get coefficients of firfarrow object
//  _f      :   firfarrow object
//  _h      :   output coefficients pointer
void FIRFARROW(_get_coefficients)(FIRFARROW() _f,
                                  TC * _h)
{
    memmove(_h, _f->h, (_f->h_len)*sizeof(TC));
}

// compute complex frequency response
//  _q      :   filter object
//  _fc     :   frequency
//  _H      :   output frequency response
void FIRFARROW(_freqresponse)(FIRFARROW() _q,
                              float _fc,
                              float complex * _H)
{
    unsigned int i;
    float complex H = 0.0f;

    for (i=0; i<_q->h_len; i++)
        H += _q->h[i] * cexpf(_Complex_I*2*M_PI*_fc*i);

    // set return value
    *_H = H;
}

// compute group delay in samples
//  _q      :   filter object
//  _fc     :   frequency
float FIRFARROW(_groupdelay)(FIRFARROW() _q,
                             float _fc)
{
    // copy coefficients to be in correct order
    float h[_q->h_len];
    unsigned int i;
    unsigned int n = _q->h_len;
    for (i=0; i<n; i++)
        h[i] = crealf(_q->h[i]);

    return fir_group_delay(h, n, _fc);
}



// 
// internal
//

// generate polynomials to represent filter coefficients
void FIRFARROW(_genpoly)(FIRFARROW() _q)
{
    // TODO : shy away from 'float' and use 'TC' types
    unsigned int i, j, n=0;
    float x, mu, h0, h1;
    float mu_vect[_q->Q+1];
    float hp_vect[_q->Q+1];
    float p[_q->Q];
    float beta = kaiser_beta_As(_q->As);
    for (i=0; i<_q->h_len; i++) {
#if FIRFARROW_DEBUG
        printf("i : %3u / %3u\n", i, _q->h_len);
#endif
        x = (float)(i) - (float)(_q->h_len-1)/2.0f;
        for (j=0; j<=_q->Q; j++) {
            mu = ((float)j - (float)_q->Q)/((float)_q->Q) + 0.5f;

            h0 = sincf(2.0f*(_q->fc)*(x + mu));
            h1 = kaiser(i,_q->h_len,beta,mu);
#if FIRFARROW_DEBUG
            printf("  %3u : x=%12.8f, mu=%12.8f, h0=%12.8f, h1=%12.8f, hp=%12.8f\n",
                    j, x, mu, h0, h1, h0*h1);
#endif

            mu_vect[j] = mu;
            hp_vect[j] = h0*h1;
        }
        POLY(_fit)(mu_vect,hp_vect,_q->Q+1,p,_q->Q+1);
#if FIRFARROW_DEBUG
        printf("  polynomial : ");
        for (j=0; j<=_q->Q; j++)
            printf("%8.4f,", p[j]);
        printf("\n");
#endif

        // copy coefficients to internal matrix
        memmove(_q->P+n, p, (_q->Q+1)*sizeof(float));
        n += _q->Q+1;
    }

#if FIRFARROW_DEBUG
    // print coefficients
    n=0;
    for (i=0; i<_q->h_len; i++) {
        printf("%3u : ", i);
        for (j=0; j<_q->Q+1; j++)
            printf("%12.4e ", _q->P[n++]);
        printf("\n");
    }
#endif

    // normalize DC gain
    _q->gamma = 1.0f;                // initialize gamma to 1
    FIRFARROW(_set_delay)(_q,0.0f); // compute filter taps with zero delay
    _q->gamma = 0.0f;                // clear gamma
    for (i=0; i<_q->h_len; i++)      // compute DC response
        _q->gamma += _q->h[i];
    _q->gamma = 1.0f / (_q->gamma);   // invert result

}

