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
// Finite impulse response filter
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// defined:
//  FIR_FILTER()    name-mangling macro
//  T               coefficients type
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

struct FIR_FILTER(_s) {
    TC * h;
    unsigned int h_len;

    WINDOW() w;
    DOTPROD() dp;

    fir_prototype p;
};

FIR_FILTER() FIR_FILTER(_create)(TC * _h, unsigned int _n)
{
    FIR_FILTER() f = (FIR_FILTER()) malloc(sizeof(struct FIR_FILTER(_s)));
    f->h_len = _n;
    f->h = (TC *) malloc((f->h_len)*sizeof(TC));

    // load filter in reverse order
    unsigned int i;
    for (i=_n; i>0; i--)
        f->h[i-1] = _h[_n-i];

    f->w = WINDOW(_create)(f->h_len);
    f->dp = DOTPROD(_create)(f->h, f->h_len);

    FIR_FILTER(_clear)(f);

    return f;
}

FIR_FILTER() FIR_FILTER(_create_prototype)(unsigned int _n)
{
    printf("warning: fir_filter_create_prototype(), not yet implemented\n");
    FIR_FILTER() f = (FIR_FILTER()) malloc(sizeof(struct FIR_FILTER(_s)));
    f->h_len = _n;
    f->h = (TC *) malloc((f->h_len)*sizeof(TC));

    // use remez here

    return f;
}

FIR_FILTER() FIR_FILTER(_recreate)(FIR_FILTER() _f, TC * _h, unsigned int _n)
{
    unsigned int i;
    if (_n != _f->h_len) {
        // reallocate memory
        _f->h_len = _n;
        _f->h = (TC*) realloc(_f->h, (_f->h_len)*sizeof(TC));

        _f->w = WINDOW(_recreate)(_f->w, _f->h_len);
    }

    // load filter in reverse order
    for (i=_n; i>0; i--)
        _f->h[i-1] = _h[_n-i];

    DOTPROD(_destroy)(_f->dp);
    _f->dp = DOTPROD(_create)(_f->h, _f->h_len);

    return _f;
}


void FIR_FILTER(_destroy)(FIR_FILTER() _f)
{
    WINDOW(_destroy)(_f->w);
    DOTPROD(_destroy)(_f->dp);
    free(_f->h);
    free(_f);
}

void FIR_FILTER(_clear)(FIR_FILTER() _f)
{
    WINDOW(_clear)(_f->w);
}

void FIR_FILTER(_print)(FIR_FILTER() _f)
{
    printf("filter coefficients:\n");
    unsigned int i, n = _f->h_len;
    for (i=0; i<n; i++) {
        printf("  h(%3u) = ", i+1);
        PRINTVAL_TC(_f->h[n-i-1],%12.8f);
        printf("\n");
    }
}

void FIR_FILTER(_push)(FIR_FILTER() _f, TI _x)
{
    WINDOW(_push)(_f->w, _x);
}

void FIR_FILTER(_execute)(FIR_FILTER() _f, TO *_y)
{
    TI *r;
    WINDOW(_read)(_f->w, &r);
    //DOTPROD(_run)(_f->h, r, _f->h_len, _y);
    DOTPROD(_execute)(_f->dp, r, _y);
}

unsigned int FIR_FILTER(_get_length)(FIR_FILTER() _f)
{
    return _f->h_len;
}

