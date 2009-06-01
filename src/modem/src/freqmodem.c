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
// Frequency modulator/demodulator
//

#include <stdlib.h>
#include <stdio.h>

#include "liquid.internal.h"

struct freqmodem_s {
    float theta;    // phase angle
    float m;        // modulation index
};

freqmodem freqmodem_create()
{
    freqmodem fm = (freqmodem) malloc(sizeof(struct freqmodem_s));

    fm->theta=0.0f;
    fm->m = 1.0f;

    return fm;
}

void freqmodem_destroy(freqmodem _fm)
{
    free(_fm);
}

void freqmodem_print(freqmodem _fm)
{
    printf("freqmodem:\n");
    printf("    mod. index  :   %8.4f\n", _fm->m);
}

void freqmodem_modulate(freqmodem _fm, float _x, float complex *_y)
{

}

void freqmodem_demodulate(freqmodem _fm, float complex *_y, float *_x)
{

}


