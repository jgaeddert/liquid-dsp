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
// auto-correlator (delay cross-correlation)
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// defined:
//  AUTOCORR()      name-mangling macro
//  TI              type (input)
//  TC              type (coefficients)
//  TO              type (output)
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

struct AUTOCORR(_s) {
    unsigned int window_size;
    unsigned int delay;

    WINDOW() w;         // input buffer
    WINDOW() wdelay;    // input buffer with delay

    float * we2;        // energy buffer
    float e2_sum;       // running sum of energy
    unsigned int ie2;   // read index

    TI * rw;            // input buffer read pointer
    TC * rwdelay;       // input buffer read pointer (with delay)
};

AUTOCORR() AUTOCORR(_create)(unsigned int _window_size,
                             unsigned int _delay)
{
    AUTOCORR() q = (AUTOCORR()) malloc(sizeof(struct AUTOCORR(_s)));
    q->window_size = _window_size;
    q->delay = _delay;

    q->w        = WINDOW(_create)(q->window_size);
    q->wdelay   = WINDOW(_create)(q->window_size + q->delay);

    q->we2      = (float*) malloc( (q->window_size)*sizeof(float) );

    // clear object
    AUTOCORR(_clear)(q);

    return q;
}

void AUTOCORR(_destroy)(AUTOCORR() _q)
{
    WINDOW(_destroy)(_q->w);
    WINDOW(_destroy)(_q->wdelay);
    free(_q->we2);
    free(_q);
}

void AUTOCORR(_clear)(AUTOCORR() _q)
{
    WINDOW(_clear)(_q->w);
    WINDOW(_clear)(_q->wdelay);
    
    _q->e2_sum = 0.0;
    unsigned int i;
    for (i=0; i<_q->window_size; i++)
        _q->we2[i] = 0.0;
    _q->ie2 = 0;
}

void AUTOCORR(_print)(AUTOCORR() _q)
{
    printf("autocorr [%u window, %u delay]\n", _q->window_size, _q->delay);
}

void AUTOCORR(_push)(AUTOCORR() _q, TI _x)
{
    WINDOW(_push)(_q->w,      _x);
    WINDOW(_push)(_q->wdelay, conj(_x));

    // push |_x|^2 into buffer
    float e2 = creal( _x*conj(_x) );
    _q->e2_sum -= _q->we2[ _q->ie2 ];
    _q->e2_sum += e2;
    _q->we2[ _q->ie2 ] = e2;
    _q->ie2 = (_q->ie2+1) % _q->window_size;
}

void AUTOCORR(_execute)(AUTOCORR() _q, TO *_rxx)
{
    WINDOW(_read)(_q->w, &_q->rw);
    WINDOW(_read)(_q->wdelay, &_q->rwdelay);

    DOTPROD(_run4)(_q->rw,_q->rwdelay,_q->window_size,_rxx);
}

float AUTOCORR(_get_energy)(AUTOCORR() _q)
{
    return _q->e2_sum;
}

