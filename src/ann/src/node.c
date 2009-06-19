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
// Artificial neural network (node definitions)
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "liquid.internal.h"

#define NODE(name)      LIQUID_CONCAT(node,name)
#define DOTPROD(name)   LIQUID_CONCAT(dotprod_rrrf,name)
#define T               float

#define DEBUG_NODE  0

NODE() NODE(_create)(float * _w,
                     float * _x,
                     float * _y,
                     unsigned int _num_inputs,
                     int _activation_func,
                     float _mu)
{
    NODE() n = (NODE()) malloc(sizeof(struct NODE(_s)));
    n->w = _w;
    n->x = _x;
    n->y = _y;
    n->num_inputs = _num_inputs;
    n->activation_func = ann_af_linear;
    n->d_activation_func = ann_df_linear;
    n->mu = _mu;
    return n;
}

void NODE(_destroy)(NODE() _n)
{
    free(_n);
}

void NODE(_print)(NODE() _n)
{
    printf("node [%u inputs]:\n", _n->num_inputs);
}

void NODE(_evaluate)(NODE() _n)
{
    T y;

    // compute output (dot product with input)
    DOTPROD(_run)(_n->x, _n->w, _n->num_inputs, &y);

    // add bias
    y += _n->w[_n->num_inputs];

    // apply activation function
    _n->y[0] = _n->activation_func(_n->mu, y);
}

