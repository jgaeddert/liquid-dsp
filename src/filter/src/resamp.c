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
// Arbitrary resampler
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// defined:
//  TO          output data type
//  TC          coefficient data type
//  TI          input data type
//  RESAMP()    name-mangling macro
//  FIRPFB()    firpfb macro

struct RESAMP(_s) {
    TC * h;
    unsigned int h_len;

    float r;        // rate
    float b_soft;   // filterbank index (soft value)

    FIRPFB() f;

    fir_prototype p;
};

RESAMP() RESAMP(_create)(float _r)
{
    RESAMP() q = (RESAMP()) malloc(sizeof(struct RESAMP(_s)));
    q->r = _r;
    return q;
}

void RESAMP(_destroy)(RESAMP() _q)
{
    free(_q);
}

void RESAMP(_print)(RESAMP() _q)
{
    printf("resampler [rate: %f]\n", _q->r);
}

void RESAMP(_execute)(RESAMP() _q)
{
}

