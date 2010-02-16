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
// Infinite impulse response filter (second-order section)
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// defined:
//  IIRFILTSOS()    name-mangling macro
//  TO              output type
//  TC              coefficients type
//  TI              input type
//  PRINTVAL()      print macro(s)

struct IIRFILTSOS(_s) {
    TC b[3];         // feedforward coefficients
    TC a[3];         // feedback coefficients
    TI v[3];         // internal filter state
};

IIRFILTSOS() IIRFILTSOS(_create)(TC * _b,
                                 TC * _a)
{
    IIRFILTSOS() q = (IIRFILTSOS()) malloc(sizeof(struct IIRFILTSOS(_s)));

    TC a0 = _a[0];

    // copy feed-forward coefficients (numerator)
    q->b[0] = _b[0] / a0;
    q->b[1] = _b[1] / a0;
    q->b[2] = _b[2] / a0;

    // copy feed-back coefficients (denominator)
    q->a[0] = _a[0] / a0;
    q->a[1] = _a[1] / a0;
    q->a[2] = _a[2] / a0;

    // clear filter state
    IIRFILTSOS(_clear)(q);

    return q;
}

void IIRFILTSOS(_destroy)(IIRFILTSOS() _q)
{
    free(_q);
}

void IIRFILTSOS(_print)(IIRFILTSOS() _q)
{
    printf("iir filter | sos:\n");

    printf("  b : ");
    PRINTVAL_TC(_q->b[0],%12.8f); printf(",");
    PRINTVAL_TC(_q->b[1],%12.8f); printf(",");
    PRINTVAL_TC(_q->b[2],%12.8f); printf("\n");

    printf("  a : ");
    PRINTVAL_TC(_q->a[0],%12.8f); printf(",");
    PRINTVAL_TC(_q->a[1],%12.8f); printf(",");
    PRINTVAL_TC(_q->a[2],%12.8f); printf("\n");
}

void IIRFILTSOS(_clear)(IIRFILTSOS() _q)
{
    // set to zero
    _q->v[0] = 0;
    _q->v[1] = 0;
    _q->v[2] = 0;
}

void IIRFILTSOS(_execute)(IIRFILTSOS() _q,
                          TI   _x,
                          TO * _y)
{
    // advance buffer
    _q->v[2] = _q->v[1];
    _q->v[1] = _q->v[0];

    // compute new v[0]
    _q->v[0] = _x - 
               _q->a[1]*_q->v[1] -
               _q->a[2]*_q->v[2];

    // compute output _y
    *_y = _q->b[0]*_q->v[0] +
          _q->b[1]*_q->v[1] +
          _q->b[2]*_q->v[2];
}

