/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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
    unsigned int delay;         // length of window
    unsigned int read_index;
};

// create delay buffer object with '_delay' samples
WDELAY() WDELAY(_create)(unsigned int _delay)
{
    // create main object
    WDELAY() q = (WDELAY()) malloc(sizeof(struct WDELAY(_s)));

    // set internal values
    q->delay = _delay;

    // allocte memory
    q->v = (T*) malloc((q->delay)*sizeof(T));
    q->read_index = 0;

    // clear window
    WDELAY(_clear)(q);

    return q;
}

// re-create delay buffer object with '_delay' samples
//  _q      :   old delay buffer object
//  _delay  :   delay for new object
WDELAY() WDELAY(_recreate)(WDELAY()     _q,
                           unsigned int _delay)
{
    // copy internal buffer, re-aligned
    unsigned int ktmp = _q->delay;
    T * vtmp = (T*) malloc(_q->delay * sizeof(T));
    unsigned int i;
    for (i=0; i<_q->delay; i++)
        vtmp[i] = _q->v[ (i + _q->read_index) % _q->delay ];
    
    // destroy object and re-create it
    WDELAY(_destroy)(_q);
    _q = WDELAY(_create)(_delay);

    // push old values
    for (i=0; i<ktmp; i++)
        WDELAY(_push)(_q, vtmp[i]);

    // free temporary array
    free(vtmp);

    // return object
    return _q;
}

// destroy delay buffer object, freeing internal memory
void WDELAY(_destroy)(WDELAY() _q)
{
    // free internal array buffer
    free(_q->v);

    // free main object memory
    free(_q);
}

// print delay buffer object's state to stdout
void WDELAY(_print)(WDELAY() _q)
{
    printf("wdelay [%u elements] :\n", _q->delay);
    unsigned int i, j;
    for (i=0; i<_q->delay; i++) {
        j = (i + _q->read_index) % _q->delay;
        printf("%4u", i);
        BUFFER_PRINT_VALUE(_q->v[j]);
        printf("\n");
    }
}

// clear/reset state of object
void WDELAY(_clear)(WDELAY() _q)
{
    _q->read_index = 0;
    memset(_q->v, 0, (_q->delay)*sizeof(T));
}

// read delayed sample from delay buffer object
//  _q  :   delay buffer object
//  _v  :   value of delayed element
void WDELAY(_read)(WDELAY() _q,
                   T *      _v)
{
    // return value at end of buffer
    *_v = _q->v[_q->read_index];
}

// push new sample into delay buffer object
//  _q  :   delay buffer object
//  _v  :   new value to be added to buffer
void WDELAY(_push)(WDELAY() _q,
                   T        _v)
{
    // append value to end of buffer
    _q->v[_q->read_index] = _v;

    // increment index
    _q->read_index++;

    // wrap around pointer
    _q->read_index %= _q->delay;
}

