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
// Finite impulse response Farrow filter
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FIR_FARROW_USE_DOTPROD 1

#define FIR_FARROW_DEBUG 0

// defined:
//  FIR_FARROW()    name-mangling macro
//  T               coefficients type
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

struct FIR_FARROW(_s) {
    TC * h;
    unsigned int h_len; // filter length
    float fc;           // filter cutoff
    unsigned int Q;     // polynomial order

    float mu;       // fractional sample delay
    float * P;      // polynomail coefficients matrix [n x p]
    float gamma;    // inverse of DC response (normalization factor)

#if FIR_FARROW_USE_DOTPROD
    WINDOW() w;
#else
    TI * v;
    unsigned int v_index;
#endif
};

FIR_FARROW() FIR_FARROW(_create)(unsigned int _n,
                                 unsigned int _p,
                                 float _beta)
{
    FIR_FARROW() f = (FIR_FARROW()) malloc(sizeof(struct FIR_FARROW(_s)));
    f->h_len = _n;
    f->h = (TC *) malloc((f->h_len)*sizeof(TC));
    f->Q = _p;
    f->fc = 0.9f;

    // TODO: validate input

    // load filter in reverse order
    unsigned int i;
    //for (i=_n; i>0; i--)
    //    f->h[i-1] = _h[_n-i];

#if FIR_FARROW_USE_DOTPROD
    f->w = WINDOW(_create)(f->h_len);
#else
    f->v = malloc((f->h_len)*sizeof(TI));
#endif

    // allocate memory for polynomial matrix
    f->P = (float*) malloc((f->h_len)*(f->Q+1)*sizeof(float));

    FIR_FARROW(_clear)(f);

    unsigned int j, n=0;
    float x, mu, h0, h1;
    float mu_vect[f->Q+1];
    float hp_vect[f->Q+1];
    float p[f->Q];
    float beta = kaiser_beta_slsl(80.0f);
    for (i=0; i<_n; i++) {
#if FIR_FARROW_DEBUG
        printf("i : %3u / %3u\n", i, _n);
#endif
        x = (float)(i) - (float)(_n-1)/2.0f;
        for (j=0; j<=_p; j++) {
            mu = ((float)j - (float)_p)/((float)_p) + 0.5f;

            h0 = sincf((f->fc)*(x + mu));
            h1 = kaiser(i,_n,beta,mu);
#if FIR_FARROW_DEBUG
            printf("  %3u : x=%12.8f, mu=%12.8f, h0=%12.8f, h1=%12.8f, hp=%12.8f\n",
                    j, x, mu, h0, h1, h0*h1);
#endif

            mu_vect[j] = mu;
            hp_vect[j] = h0*h1;
        }
        polyfit(mu_vect,hp_vect,f->Q+1,p,f->Q+1);
#if FIR_FARROW_DEBUG
        printf("  polynomial : ");
        for (j=0; j<=_p; j++)
            printf("%8.4f,", p[j]);
        printf("\n");
#endif

        // copy coefficients to internal matrix
        memmove(f->P+n, p, (f->Q+1)*sizeof(float));
        n += f->Q+1;
    }

#if FIR_FARROW_DEBUG
    // print coefficients
    n=0;
    for (i=0; i<f->h_len; i++) {
        printf("%3u : ", i);
        for (j=0; j<f->Q+1; j++)
            printf("%12.4e ", f->P[n++]);
        printf("\n");
    }
#endif

    // normalize DC gain
    f->gamma = 1.0f;                // initialize gamma to 1
    FIR_FARROW(_set_delay)(f,0.0f); // compute filter taps with zero delay
    f->gamma = 0.0f;                // clear gamma
    for (i=0; i<f->h_len; i++)      // compute DC response
        f->gamma += f->h[i];
    f->gamma = 1.0f / (f->gamma);   // invert result

    return f;
}

void FIR_FARROW(_destroy)(FIR_FARROW() _f)
{
#if FIR_FARROW_USE_DOTPROD
    WINDOW(_destroy)(_f->w);
#else
    free(_f->v);
#endif
    free(_f->h);
    free(_f->P);
    free(_f);
}

void FIR_FARROW(_clear)(FIR_FARROW() _f)
{
#if FIR_FARROW_USE_DOTPROD
    WINDOW(_clear)(_f->w);
#else
    unsigned int i;
    for (i=0; i<_f->h_len; i++)
        _f->v[i] = 0;
    _f->v_index = 0;
#endif
}

void FIR_FARROW(_print)(FIR_FARROW() _f)
{
    printf("fir_farrow [len : %u, poly-order : %u]\n", _f->h_len, _f->Q);
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
        PRINTVAL(_f->h[n-i-1]);
        printf("\n");
    }
}

void FIR_FARROW(_push)(FIR_FARROW() _f, TI _x)
{
#if FIR_FARROW_USE_DOTPROD
    WINDOW(_push)(_f->w, _x);
#else
    _f->v[ _f->v_index ] = _x;
    (_f->v_index)++;
    _f->v_index = (_f->v_index) % (_f->h_len);
#endif
}

void FIR_FARROW(_set_delay)(FIR_FARROW() _f, float _mu)
{
    // TODO: validate input

    unsigned int i, n=0;
    for (i=0; i<_f->h_len; i++) {
        // compute filter tap from polynomial using negative
        // value for _mu
        _f->h[i] = polyval(_f->P+n, _f->Q, -_mu);

        // normalize filter by inverse of DC response
        _f->h[i] *= _f->gamma;

        n += _f->Q+1;

        //printf("  h[%3u] = %12.8f\n", i, _f->h[i]);
    }
}

void FIR_FARROW(_execute)(FIR_FARROW() _f, TO *_y)
{
#if FIR_FARROW_USE_DOTPROD
    TI *r;
    WINDOW(_read)(_f->w, &r);
    DOTPROD(_run)(_f->h, r, _f->h_len, _y);
#else
    TO y = 0;
    unsigned int i;
    for (i=0; i<_f->h_len; i++)
        y += _f->v[ (i+_f->v_index)%(_f->h_len) ] * _f->h[i];
    *_y = y;
#endif
}

unsigned int FIR_FARROW(_get_length)(FIR_FARROW() _f)
{
    return _f->h_len;
}

void FIR_FARROW(_get_coefficients)(FIR_FARROW() _f, float * _h)
{
    memmove(_h, _f->h, (_f->h_len)*sizeof(float));
}

