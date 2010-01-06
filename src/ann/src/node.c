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
    n->activation_func = ann_af_tanh;
    n->d_activation_func = ann_df_tanh;
    n->mu = _mu;
    return n;
}

void NODE(_destroy)(NODE() _n)
{
    free(_n);
}

void NODE(_print)(NODE() _n)
{
    printf("  node [%u inputs]:\n", _n->num_inputs);
    unsigned int i;
    for (i=0; i<_n->num_inputs; i++)
        printf("    w[%3u] = %12.8f : x[%3u] = %12.8f\n", i, _n->w[i], i, _n->x[i]);
    printf("    w[%3u] = %12.8f (bias)\n", i, _n->w[i]);
    printf("    v = %12.8f\n", _n->v);
    printf("    y = %12.8f\n", _n->y[0]);
}

void NODE(_evaluate)(NODE() _n)
{
    // compute output (dot product with input)
    DOTPROD(_run)(_n->x, _n->w, _n->num_inputs, &(_n->v));

    // add bias
    _n->v += _n->w[_n->num_inputs];

    // apply activation function
    _n->y[0] = _n->activation_func(_n->mu, _n->v);
}

// compute back-propagation error
void NODE(_compute_bp_error)(NODE() _n, T _error)
{
    // store error value
    _n->e = _error;

    // compute gradient of activation function
    _n->g = _n->d_activation_func(_n->mu, _n->v);

    // delta: output error scaled by gradient of activation function
    _n->delta = _n->e * _n->g;

#if DEBUG_NODE
    printf("  node: e : %12.8f, g : %12.8f, delta=e*g : %12.8f\n",
            _n->e, _n->g, _n->delta);
#endif
}

// TODO: update NODE(_train) to work for both input and output layers
void NODE(_train)(NODE() _n, float _eta)
{
    // update internal weights
    T dw;   // weight correction
    unsigned int i;
    for (i=0; i<_n->num_inputs; i++) {
        dw = _eta * _n->delta * _n->x[i];
        _n->w[i] += dw;
    }

    // update bias
    dw = _eta * _n->delta;
    _n->w[i] += dw;
}

