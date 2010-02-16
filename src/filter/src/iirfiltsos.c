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
// Infinite impulse response filter (second-order sections)
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

void IIRFILTSOS(_init)(IIRFILTSOS() _q,
                       TC * _b,
                       TC * _a)
{
    memmove(_q->b, _b, 3*sizeof(TC));
    memmove(_q->a, _a, 3*sizeof(TC));

    TC a0 = _a[0];

    _q->b[0] /= a0;
    _q->b[1] /= a0;
    _q->b[2] /= a0;

    _q->a[0] /= a0;
    _q->a[1] /= a0;
    _q->a[2] /= a0;

    _q->v[0] = 0;
    _q->v[1] = 0;
    _q->v[2] = 0;
}

void IIRFILTSOS(_print)(IIRFILTSOS() _q)
{
    printf("iir filter | sos:\n");
    printf("  b[0] : ");    PRINTVAL_TC(_q->b[0],%12.8f);
    printf("  b[1] : ");    PRINTVAL_TC(_q->b[1],%12.8f);
    printf("  b[2] : ");    PRINTVAL_TC(_q->b[2],%12.8f);

    printf("  a[0] : ");    PRINTVAL_TC(_q->a[0],%12.8f);
    printf("  a[1] : ");    PRINTVAL_TC(_q->a[1],%12.8f);
    printf("  a[2] : ");    PRINTVAL_TC(_q->a[2],%12.8f);
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
    _q->v[0] = _x - _q->a[1]*_q->v[1] - _q->a[2]*_q->v[2];

    // compute new y
    *_y = _q->b[0]*_q->v[0] +
          _q->b[1]*_q->v[1] +
          _q->b[2]*_q->v[2];
}

