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
// modem_ook.c
//

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.internal.h"

// create an ook (on/off keying) modem object
MODEM() MODEM(_create_ook)()
{
    MODEM() q = (MODEM()) malloc( sizeof(struct MODEM(_s)) );
    q->scheme = LIQUID_MODEM_OOK;

    MODEM(_init)(q, 1);

    q->modulate_func   = &MODEM(_modulate_ook);
    q->demodulate_func = &MODEM(_demodulate_ook);

    // reset and return
    MODEM(_reset)(q);
    return q;
}

// modulate symbol using on/off keying
void MODEM(_modulate_ook)(MODEM()         _q,
                          unsigned int    _sym_in,
                          float complex * _y)
{
    // compute output sample directly from input
    *_y = _sym_in ? 0.0f : M_SQRT2;
}

// demodulate OOK
void MODEM(_demodulate_ook)(MODEM()        _q,
                            float complex  _x,
                            unsigned int * _sym_out)
{
    // slice directly to output symbol
    *_sym_out = (crealf(_x) > M_SQRT1_2 ) ? 0 : 1;

    // re-modulate symbol and store state
    MODEM(_modulate_ook)(_q, *_sym_out, &_q->x_hat);
    _q->r = _x;
}

