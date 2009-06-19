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

#define ANN(name)       LIQUID_CONCAT(ann,name)
#define DOTPROD(name)   LIQUID_CONCAT(dotprod_rrrf,name)
#define T               float

#define LIQUID_ANN_MAX_NETWORK_SIZE 1024

#define DEBUG_ANN 0

struct ANN(_node_s) {
    T * w;      // weights vector pointer
    T * x;      // input vector pointer
    T * y;      // output vector pointer
    unsigned int num_inputs;

    // activation function (derivative) pointer
    T(*activation_func)(float,T);
    T(*d_activation_func)(float,T);

    float mu;   // activation function gain
};

typedef struct ANN(_node_s) * ANN(_node);

void ANN(_node_evaluate)(ANN(_node) _n)
{
    T y;

    // compute output (dot product with input)
    DOTPROD(_run)(_n->x, _n->w, _n->num_inputs, &y);

    // add bias
    y += _n->w[_n->num_inputs];

    // apply activation function
    _n->y[0] = _n->activation_func(_n->mu, y);
}

