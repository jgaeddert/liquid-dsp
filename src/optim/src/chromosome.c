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
// chromosome.c
//

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

#define LIQUID_CHROMOSOME_MAX_SIZE (32)

chromosome chromosome_create(unsigned int _num_parameters,
                             unsigned int _bits_per_parameter)
{
    chromosome c;
    c = (chromosome) malloc( sizeof(struct chromosome_s) );

    c->num_parameters = _num_parameters;
    c->s = (unsigned long*) calloc( sizeof(unsigned long), c->num_parameters );

    if (_bits_per_parameter >= LIQUID_CHROMOSOME_MAX_SIZE) {
        printf("warning: chromosome_create(), truncating bits_per_parameter to maximum (%d)\n",
                LIQUID_CHROMOSOME_MAX_SIZE);
        c->bits_per_parameter = LIQUID_CHROMOSOME_MAX_SIZE;
        c->max_int_value = LONG_MAX;
    } else {
        c->bits_per_parameter = _bits_per_parameter;
        c->max_int_value = (1 << c->bits_per_parameter) - 1;
    }
    c->num_bits = c->bits_per_parameter * c->num_parameters;
    c->scaling_factor = 1 / (float) (c->max_int_value);

    //printf("max_int_value: %ld\n", c->max_int_value);
    //printf("bits_per_parameter: %d\n", c->bits_per_parameter);
    //printf("scaling_factor: %E\n", c->scaling_factor);
    
    return c;
}

void chromosome_destroy(chromosome _c)
{
    free(_c->s);
    free(_c);
}

void chromosome_print(chromosome _c)
{
    unsigned int i;
    for (i=0; i<_c->num_parameters; i++)
        printf("%6.3f ", chromosome_value(_c, i));

    printf("\n");
}

void chromosome_mutate(chromosome _c, unsigned int _index)
{
    // ensure _index does not exceed maximum
    _index = (_index >= _c->num_bits) ? 0 : _index;

    div_t d = div(_index, _c->bits_per_parameter);
    _c->s[d.quot] ^= (unsigned long) (1 << d.rem);
}

void chromosome_crossover(chromosome _p1,
                          chromosome _p2,
                          chromosome _c,
                          unsigned int _threshold)
{
    // TODO : ensure _p1, _p2, and _c are all the same length

    // ensure _threshold does not exceed maximum
    _threshold = (_threshold > _c->num_bits) ? 0 : _threshold;

    div_t d = div(_threshold, _c->bits_per_parameter);

    unsigned int i;
    for (i=0; i<d.quot; i++)
        _c->s[i] = _p1->s[i];

    for (i=d.quot; i<_c->num_parameters; i++)
        _c->s[i] = _p2->s[i];

    // handle condition where crossover lies within a single parameter
    if (d.rem != 0) {
        unsigned long mask = (1 << d.rem) - 1;
        mask <<= _c->bits_per_parameter - d.rem;
        _c->s[d.quot] = (_p1->s[d.quot] & mask) | (_p2->s[d.quot] & ~mask);
    }

}
    
void chromosome_init_random(chromosome _c)
{
    unsigned int i;
    for (i=0; i<_c->num_parameters; i++)
        (_c->s)[i] = rand() % _c->max_int_value;

}

float chromosome_value(chromosome _c, unsigned int _index)
{
    // translate v from integer to float on [0,1]
    float v = (_c->s)[_index] * _c->scaling_factor;

    if (1)
        // apply activation function; expanding [0,1] to [-4,4]
        return 8.0f*(v-0.5f);
    else
        return v;
}

