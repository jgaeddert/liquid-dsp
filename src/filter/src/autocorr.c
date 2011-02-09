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

    windowf we2;        // energy buffer

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

    q->we2      = windowf_create(q->window_size);

    return q;
}

void AUTOCORR(_destroy)(AUTOCORR() _q)
{
    WINDOW(_destroy)(_q->w);
    WINDOW(_destroy)(_q->wdelay);
    windowf_destroy(_q->we2);
    free(_q);
}

void AUTOCORR(_clear)(AUTOCORR() _q)
{
    WINDOW(_clear)(_q->w);
    WINDOW(_clear)(_q->wdelay);
    windowf_clear(_q->we2);
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
    windowf_push(_q->we2, crealf( _x*conjf(_x) ));
}

void AUTOCORR(_execute)(AUTOCORR() _q, TO *_rxx)
{
    WINDOW(_read)(_q->w, &_q->rw);
    WINDOW(_read)(_q->wdelay, &_q->rwdelay);

    DOTPROD(_run)(_q->rw,_q->rwdelay,_q->window_size,_rxx);
}

float AUTOCORR(_get_energy)(AUTOCORR() _q)
{
    // normalize by energy (slow method)
    unsigned int i;
    float e2 = 0.0f;
    float * r;
    windowf_read(_q->we2, &r);

    for (i=0; i<_q->window_size; i++)
        e2 += r[i];

    return e2;
}

