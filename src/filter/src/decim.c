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
// decim.c
//
// finite impulse response decimator object definitions
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// defined:
//  DECIM()     name-mangling macro
//  TO          output data type
//  TC          coefficients data type
//  TI          input data type
//  WINDOW()    window macro
//  DOTPROD()   dotprod macro
//  PRINTVAL()  print macro

struct DECIM(_s) {
    TC * h;
    unsigned int h_len;
    unsigned int D;

    WINDOW() w;
    DOTPROD() dp;

#if 0
    fir_prototype p;    // prototype object
#endif
};

// create decimator object
//  _D      :   decimation factor
//  _h      :   filter coefficients array pointer
//  _h_len  :   length of filter
DECIM() DECIM(_create)(unsigned int _D,
                       TC *_h,
                       unsigned int _h_len)
{
    // validate input
    if (_h_len == 0) {
        fprintf(stderr,"error: decim_xxxt_create(), filter length must be greater than zero\n");
        exit(1);
    } else if (_D == 0) {
        fprintf(stderr,"error: decim_xxxt_create(), decimation factor must be greater than zero\n");
        exit(1);
    }

    DECIM() q = (DECIM()) malloc(sizeof(struct DECIM(_s)));
    q->h_len = _h_len;
    q->D = _D;
    q->h = (TC*) malloc((q->h_len)*sizeof(TC));

    // create window (internal buffer)
    q->w = WINDOW(_create)(q->h_len);

    // load filter in reverse order
    unsigned int i;
    for (i=0; i<q->h_len; i++)
        q->h[i] = _h[_h_len-i-1];

    // create dot product object
    q->dp = DOTPROD(_create)(q->h, q->h_len);

    // reset filter state (clear buffer)
    DECIM(_clear)(q);

    return q;
}

// destroy decimator object
void DECIM(_destroy)(DECIM() _q)
{
    WINDOW(_destroy)(_q->w);
    DOTPROD(_destroy)(_q->dp);
    free(_q->h);
    free(_q);
}

// print decimator object internals
void DECIM(_print)(DECIM() _q)
{
    printf("DECIM() [%u] :\n", _q->D);
    printf("  window:\n");
    WINDOW(_print)(_q->w);
}

// clear decimator object
void DECIM(_clear)(DECIM() _q)
{
    WINDOW(_clear)(_q->w);
}

// execute decimator
//  _q      :   decimator object
//  _x      :   input sample array
//  _y      :   output sample pointer
//  _index  :   output sample phase
void DECIM(_execute)(DECIM() _q,
                     TI *_x,
                     TO *_y,
                     unsigned int _index)
{
    // validate input
    if (_index >= _q->D) {
        fprintf(stderr,"error: decim_xxxt_execute(), output sample phase exceeds decimation factor\n");
        exit(1);
    }

    TI * r; // read pointer
    unsigned int i;
    for (i=0; i<_q->D; i++) {
        WINDOW(_push)(_q->w, _x[i]);

        if (i==_index) {
            // read buffer (retrieve pointer to aligned memory array)
            WINDOW(_read)(_q->w, &r);

            // execute dot product
            DOTPROD(_execute)(_q->dp, r, _y);
        }
    }
}

