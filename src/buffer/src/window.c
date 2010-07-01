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
// Windows, defined by macro
//

#include "liquid.internal.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct WINDOW(_s) {
    T * v;                      // allocated array pointer
    unsigned int len;           // length of window
    unsigned int m;             // floor(log2(len)) + 1
    unsigned int n;             // 2^m
    unsigned int mask;          // n-1
    unsigned int N;             // number of elements allocated
                                // in memory
    unsigned int read_index;
};

WINDOW() WINDOW(_create)(unsigned int _n)
{
    WINDOW() w = (WINDOW()) malloc(sizeof(struct WINDOW(_s)));
    w->len = _n;

    w->m = liquid_msb_index(_n);    // effectively floor(log2(len))+1
    w->n = 1<<(w->m);       // 
    w->mask = w->n - 1;     // bit mask

    // number of elements to allocate to memory
    w->N = w->n + w->len - 1;

    // allocte memory
    w->v = (T*) malloc((w->N)*sizeof(T));
    w->read_index = 0;

    // clear window
    WINDOW(_clear)(w);

    return w;
}

WINDOW() WINDOW(_recreate)(WINDOW() _w, unsigned int _n)
{
    // TODO: only create new window if old is too small
    
    if (_n == _w->len)
        return _w;

    // create new window
    WINDOW() w = WINDOW(_create)(_n);

    // copy old values
    T* r;
    WINDOW(_read)(_w, &r);
    //memmove(w->v, ...);
    unsigned int i;
    if (_n > _w->len) {
        // new buffer is larger; push zeros, then old values
        for (i=0; i<(_n-_w->len); i++)
            WINDOW(_push)(w, 0);
        for (i=0; i<_w->len; i++)
            WINDOW(_push)(w, r[i]);
    } else {
        // new buffer is shorter; push latest old values
        for (i=(_w->len-_n); i<_w->len; i++)
            WINDOW(_push)(w, r[i]);
    }

    // destroy old window
    WINDOW(_destroy)(_w);

    return w;
}

void WINDOW(_destroy)(WINDOW() _w)
{
    free(_w->v);
    free(_w);
}

void WINDOW(_print)(WINDOW() _w)
{
    printf("window [%u elements] :\n", _w->len);
    unsigned int i;
    T * r;
    WINDOW(_read)(_w, &r);
    for (i=0; i<_w->len; i++) {
        printf("%4u", i);
        BUFFER_PRINT_VALUE(r[i]);
        printf("\n");
    }
}

void WINDOW(_debug_print)(WINDOW() _w)
{
    printf("window [%u elements] :\n", _w->len);
    unsigned int i;
    for (i=0; i<_w->len; i++) {
        // print read index pointer
        if (i==_w->read_index)
            printf("<r>");

        // print window value
        BUFFER_PRINT_LINE(_w,i)
        printf("\n");
    }
    printf("----------------------------------\n");

    // print excess window memory
    for (i=_w->len; i<_w->N; i++) {
        BUFFER_PRINT_LINE(_w,i)
        printf("\n");
    }
}

void WINDOW(_clear)(WINDOW() _w)
{
    _w->read_index = 0;
    memset(_w->v, 0, (_w->len)*sizeof(T));
}

void WINDOW(_read)(WINDOW() _w, T ** _v)
{
    // return pointer to buffer
    *_v = _w->v + _w->read_index;
}

void WINDOW(_index)(WINDOW() _w,
                    unsigned int _i,
                    T * _v)
{
    // validate input
    if (_i >= _w->len) {
        fprintf(stderr,"error: window_index(), index value out of range\n");
        exit(1);
    }

    // return value at index
    *_v = _w->v[_w->read_index + _i];
}

void WINDOW(_push)(WINDOW() _w, T _v)
{
    // increment index
    _w->read_index++;

    // wrap around pointer
    _w->read_index &= _w->mask;

    // if pointer wraps around, copy excess memory
    if (_w->read_index == 0)
        memmove(_w->v, _w->v + _w->n, (_w->len)*sizeof(T));

    // append value to end of buffer
    _w->v[_w->read_index + _w->len - 1] = _v;
}

void WINDOW(_write)(WINDOW() _w, T * _v, unsigned int _n)
{
    // TODO make this more efficient
    unsigned int i;
    for (i=0; i<_n; i++)
        WINDOW(_push)(_w, _v[i]);
}

