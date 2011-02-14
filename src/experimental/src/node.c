/*
 * Copyright (c) 2007, 2009, 2011 Joseph Gaeddert
 * Copyright (c) 2007, 2009, 2011 Virginia Polytechnic 
 *                                Institute & State University
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

#include "liquid.experimental.h"

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
    switch (_activation_func) {
    case LIQUID_ANN_AF_LINEAR:
        n->activation_func = ann_af_linear;
        n->d_activation_func = ann_df_linear;
        break;
    case LIQUID_ANN_AF_LOGISTIC:
        n->activation_func = ann_af_logistic;
        n->d_activation_func = ann_df_logistic;
        break;
    case LIQUID_ANN_AF_TANH:
        n->activation_func = ann_af_tanh;
        n->d_activation_func = ann_df_tanh;
        break;
    case LIQUID_ANN_AF_MULAW:
        n->activation_func = ann_af_mulaw;
        n->d_activation_func = ann_df_mulaw;
        break;
    case LIQUID_ANN_AF_ERF:
        n->activation_func = ann_af_erf;
        n->d_activation_func = ann_df_erf;
        break;
    default:
        printf("error: node_create(), invalid activation function %d\n", _activation_func);
        exit(1);
    }
    n->mu = _mu;

    // back-propagation weight correction array
    n->dw = (T*) malloc( (n->num_inputs+1)*sizeof(T) );
    unsigned int i;
    for (i=0; i<n->num_inputs+1; i++)
        n->dw[i] = 0.0;

    return n;
}

void NODE(_destroy)(NODE() _n)
{
    // free weight correction array (back-propagation)
    free(_n->dw);

    // free main object memory
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

// update weights with learning rate _eta and momentum constant _alpha
void NODE(_train)(NODE() _n, float _eta)
{
    // update internal weights
    float alpha = 0.2f; // momentum constant
    T dw_hat;           // weight correction estimate
    T input;            // neuron input
    unsigned int i;
    for (i=0; i<_n->num_inputs+1; i++) {
        // determine input value (bias?)
        input = (i == _n->num_inputs) ? 1.0f : _n->x[i];

        // compute weight correction estimate
        dw_hat = _eta * _n->delta * input;

        // compute weight correction using momentum constant (retain
        // a small portion of previous correction)
        _n->dw[i] = dw_hat + alpha*_n->dw[i];

        // update weights
        _n->w[i] += _n->dw[i];
    }

}

