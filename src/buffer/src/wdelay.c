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
// windowed delay, defined by macro
//

#include "liquid.internal.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct WDELAY(_s) {
    T * v;                      // allocated array pointer
    unsigned int k;             // length of window
    unsigned int read_index;
};

WDELAY() WDELAY(_create)(unsigned int _k)
{
    WDELAY() w = (WDELAY()) malloc(sizeof(struct WDELAY(_s)));
    w->k = _k;

    // allocte memory
    w->v = (T*) malloc((w->k)*sizeof(T));
    w->read_index = 0;

    // clear window
    WDELAY(_clear)(w);

    return w;
}

WDELAY() WDELAY(_recreate)(WDELAY() _w,
                           unsigned int _k)
{
    // copy internal buffer, re-aligned
    unsigned int ktmp = _w->k;
    T * vtmp = (T*) malloc(_w->k * sizeof(T));
    unsigned int i;
    for (i=0; i<_w->k; i++)
        vtmp[i] = _w->v[ (i + _w->read_index) % _w->k ];
    
    // destroy object and re-create it
    WDELAY(_destroy)(_w);
    _w = WDELAY(_create)(_k);

    // push old values
    for (i=0; i<ktmp; i++)
        WDELAY(_push)(_w, vtmp[i]);

    // free temporary array
    free(vtmp);

    // return object
    return _w;
}

void WDELAY(_destroy)(WDELAY() _w)
{
    free(_w->v);
    free(_w);
}

void WDELAY(_print)(WDELAY() _w)
{
    printf("wdelay [%u elements] :\n", _w->k);
    unsigned int i, j;
    for (i=0; i<_w->k; i++) {
        j = (i + _w->read_index) % _w->k;
        printf("%4u", i);
        BUFFER_PRINT_VALUE(_w->v[j]);
        printf("\n");
    }
}

void WDELAY(_clear)(WDELAY() _w)
{
    _w->read_index = 0;
    memset(_w->v, 0, (_w->k)*sizeof(T));
}

void WDELAY(_read)(WDELAY() _w, T * _v)
{
    // return value at end of buffer
    *_v = _w->v[_w->read_index];
}

void WDELAY(_push)(WDELAY() _w, T _v)
{
    // append value to end of buffer
    _w->v[_w->read_index] = _v;

    // increment index
    _w->read_index++;

    // wrap around pointer
    _w->read_index %= _w->k;
}

