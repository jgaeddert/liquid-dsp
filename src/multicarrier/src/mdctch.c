/*
 * Copyright (c) 2011 Joseph Gaeddert
 * Copyright (c) 2011 Virginia Polytechnic Institute & State University
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
// mdctch.c
//
// modified discrete cosine transform channelizer
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "liquid.internal.h"

#define DEBUG_MDCTCH            1

struct mdctch_s {
    unsigned int M;         // number of channels

};

mdctch mdctch_create(unsigned int _num_channels,
                     int _type,
                     int _wtype,
                     float _beta)
{
    // validate input

    mdctch q = (mdctch) malloc(sizeof(struct mdctch_s));

    return q;
}

void mdctch_destroy(mdctch _q)
{
    free(_q);
}

void mdctch_clear(mdctch _q)
{
}

void mdctch_execute(float * _x,
                    float * _y)
{
}

void mdctch_execute_synthesizer(float * _x,
                                float * _y)
{
}

void mdctch_execute_analyzer(float * _x,
                             float * _y)
{
}

