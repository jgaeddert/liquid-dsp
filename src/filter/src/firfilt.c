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
// firfilt : finite impulse response (FIR) filter
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// defined:
//  FIRFILT()       name-mangling macro
//  T               coefficients type
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

struct FIRFILT(_s) {
    TC * h;
    unsigned int h_len;

    WINDOW() w;
    DOTPROD() dp;

    fir_prototype p;
};

FIRFILT() FIRFILT(_create)(TC * _h, unsigned int _n)
{
    FIRFILT() f = (FIRFILT()) malloc(sizeof(struct FIRFILT(_s)));
    f->h_len = _n;
    f->h = (TC *) malloc((f->h_len)*sizeof(TC));

    // load filter in reverse order
    unsigned int i;
    for (i=_n; i>0; i--)
        f->h[i-1] = _h[_n-i];

    f->w = WINDOW(_create)(f->h_len);
    f->dp = DOTPROD(_create)(f->h, f->h_len);

    FIRFILT(_clear)(f);

    return f;
}

FIRFILT() FIRFILT(_create_prototype)(unsigned int _n)
{
    fprintf(stderr,"warning: firfilt_xxxt_create_prototype(), not yet implemented\n");
    FIRFILT() f = (FIRFILT()) malloc(sizeof(struct FIRFILT(_s)));
    f->h_len = _n;
    f->h = (TC *) malloc((f->h_len)*sizeof(TC));

    // use remez here

    return f;
}

FIRFILT() FIRFILT(_recreate)(FIRFILT() _f, TC * _h, unsigned int _n)
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


void FIRFILT(_destroy)(FIRFILT() _f)
{
    WINDOW(_destroy)(_f->w);
    DOTPROD(_destroy)(_f->dp);
    free(_f->h);
    free(_f);
}

void FIRFILT(_clear)(FIRFILT() _f)
{
    WINDOW(_clear)(_f->w);
}

void FIRFILT(_print)(FIRFILT() _f)
{
    printf("filter coefficients:\n");
    unsigned int i, n = _f->h_len;
    for (i=0; i<n; i++) {
        printf("  h(%3u) = ", i+1);
        PRINTVAL_TC(_f->h[n-i-1],%12.8f);
        printf("\n");
    }
}

void FIRFILT(_push)(FIRFILT() _f, TI _x)
{
    WINDOW(_push)(_f->w, _x);
}

void FIRFILT(_execute)(FIRFILT() _f, TO *_y)
{
    TI *r;
    WINDOW(_read)(_f->w, &r);
    //DOTPROD(_run)(_f->h, r, _f->h_len, _y);
    DOTPROD(_execute)(_f->dp, r, _y);
}

unsigned int FIRFILT(_get_length)(FIRFILT() _f)
{
    return _f->h_len;
}

