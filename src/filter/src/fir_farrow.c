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

// defined:
//  FIR_FARROW()    name-mangling macro
//  T               coefficients type
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

struct FIR_FARROW(_s) {
    TC * h;
    unsigned int h_len;

    float mu;       // fractional sample delay
    TC * c;         // polynomial coefficients matrix [n x p]
    float * mu_p;   // mu^p vector

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

    // load filter in reverse order
    unsigned int i;
    //for (i=_n; i>0; i--)
    //    f->h[i-1] = _h[_n-i];

#if FIR_FARROW_USE_DOTPROD
    f->w = WINDOW(_create)(f->h_len);
#else
    f->v = malloc((f->h_len)*sizeof(TI));
#endif

    FIR_FARROW(_clear)(f);

    unsigned int j;
    float x, mu, h0, h1;
    //float mu_vect[_p+1];
    for (i=0; i<_n; i++) {
        printf("i : %3u / %3u\n", i, _n);
        x = (float)(i) - (float)(_n-1)/2.0f;
        for (j=0; j<=_p; j++) {
            mu = ((float)j - (float)_p)/((float)_p) + 0.5f;

            h0 = sincf(x + mu);
            h1 = kaiser(i,_n,10.0f,mu);
            printf("  %3u : x=%12.8f, mu=%12.8f, h0=%12.8f, h1=%12.8f\n",
                    j, x, mu, h0, h1);
        }
    }

    return f;
}

#if 0
FIR_FARROW() FIR_FARROW(_create_prototype)(unsigned int _n)
{
    printf("warning: fir_filter_create_prototype(), not yet implemented\n");
    FIR_FARROW() f = (FIR_FARROW()) malloc(sizeof(struct FIR_FARROW(_s)));
    f->h_len = _n;
    f->h = (TC *) malloc((f->h_len)*sizeof(TC));

    // use remez here

    return f;
}

FIR_FARROW() FIR_FARROW(_recreate)(FIR_FARROW() _f, TC * _h, unsigned int _n)
{
    unsigned int i;
    if (_n != _f->h_len) {
        // reallocate memory
        _f->h_len = _n;
        _f->h = (TC*) realloc(_f->h, (_f->h_len)*sizeof(TC));

#if FIR_FARROW_USE_DOTPROD
        _f->w = WINDOW(_recreate)(_f->w, _f->h_len);
#else
    _f->v = (TI*) realloc(_f->v, (_f->h_len)*sizeof(TI));
    // TODO: (bug) ensure window has proper state
    for (i=_n; i<_f->h_len; i++)
        _f->v[i] = 0;

    if (_n > _f->h_len)
        _f->v_index += (_n - _f->h_len)/2;
    else
        _f->v_index += _f->h_len + (_f->h_len - _n)/2;

    _f->v_index = (_f->v_index) % (_f->h_len);
#endif
    }

    // load filter in reverse order
    for (i=_n; i>0; i--)
        _f->h[i-1] = _h[_n-i];

    return _f;
}
#endif

void FIR_FARROW(_destroy)(FIR_FARROW() _f)
{
#if FIR_FARROW_USE_DOTPROD
    WINDOW(_destroy)(_f->w);
#else
    free(_f->v);
#endif
    free(_f->h);
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
    printf("filter coefficients:\n");
    unsigned int i, n = _f->h_len;
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

