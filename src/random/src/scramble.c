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
// Finite impulse response filter design
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

void scramble_data(unsigned char * _x, unsigned int _len)
{
    // for now apply static mask
    unsigned char mask = 0xb4;
    unsigned int i;
    for (i=0; i<_len; i++)
        _x[i] ^= mask;
}

void unscramble_data(unsigned char * _x, unsigned int _len)
{
    // for now apply simple static mask (re-run scramble)
    scramble_data(_x,_len);
}

