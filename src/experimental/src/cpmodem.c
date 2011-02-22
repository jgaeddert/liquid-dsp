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
// cpmodem.c : continuous phase modulator/demodulator
//

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "liquid.internal.h"

struct cpmodem_s {
    float * h;  // pulse shape filter
};

cpmodem cpmodem_create(cpmodem_scheme _scheme,
                       unsigned int _bits_per_symbol,
                       unsigned int _samples_per_symbol)
{
    if (_bits_per_symbol < 1 ) {
        perror("ERROR! modem_create, modem must have at least 1 bit/symbol\n");
        return NULL;
    } else if (_bits_per_symbol > MAX_MOD_BITS_PER_SYMBOL) {
        perror("ERROR! modem_create, maximum number of bits/symbol exceeded\n");
        return NULL;
    }

    switch (_scheme) {
    case CPMOD_FSK:
        return NULL;
    case CPMOD_MSK:
        return NULL;
    default:
        perror("ERROR: cpmodem_create(), unknown/unsupported modulation scheme\n");
        exit(-1);
    }

    // should never get to this point, but adding return statment
    // to keep compiler happy
    return NULL;
}

void cpmodem_destroy(cpmodem _modem)
{
}

void cpmodem_print(cpmodem _modem)
{
    printf("cpmodem:\n");
}

void cpmodem_modulate(cpmodem _modem,
                      unsigned int _s,
                      float complex * _y)
{
}

void cpmodem_demodulate(cpmodem _modem,
                        float complex * _x,
                        unsigned int * _s)
{
}

