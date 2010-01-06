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
// Artificial neural network (layer definitions)
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "liquid.internal.h"

#define ANNLAYER(name)  LIQUID_CONCAT(annlayer,name)
#define NODE(name)      LIQUID_CONCAT(node,name)
#define DOTPROD(name)   LIQUID_CONCAT(dotprod_rrrf,name)
#define T               float

#define DEBUG_ANNLAYER  0

struct ANNLAYER(_s) {
    unsigned int num_inputs;    // number of inputs into this layer
    unsigned int num_nodes;     // number of nodes in this layer
    NODE() * nodes;             // nodes in this layer
    int is_output_layer;        // output layer flag
    int is_input_layer;         // input layer flag

    float * w;                  // weights pointer (not allocated)

    float * x;                  // input array
    float * v;                  // intermediate output array
    float * y;                  // output array

    ANNLAYER() * prev_layer;    // pointer to previous layer in network
    ANNLAYER() * next_layer;    // pointer to next layer in network
};

ANNLAYER() ANNLAYER(_create)(float * _w,
                             float * _x,
                             float * _y,
                             unsigned int _num_inputs,
                             unsigned int _num_outputs,
                             int _activation_func,
                             float _mu)
{
    ANNLAYER() q = (ANNLAYER()) malloc(sizeof(struct ANNLAYER(_s)));
    
    q->num_inputs = _num_inputs;
    q->num_nodes  = _num_outputs;

    q->nodes = (NODE()*) malloc( q->num_nodes * sizeof(NODE()) );
    unsigned int i, nw=0, ny=0;
    for (i=0; i<q->num_nodes; i++) {
        q->nodes[i] = NODE(_create)(&_w[nw], _x, &_y[ny], q->num_inputs, 0, 1.0f);
        nw += q->num_inputs+1;
        ny++;
    }

    return q;
}

void ANNLAYER(_destroy)(ANNLAYER() _q)
{
    unsigned int i;
    for (i=0; i<_q->num_nodes; i++)
        NODE(_destroy)(_q->nodes[i]);
    free(_q->nodes);
    free(_q);
}

void ANNLAYER(_print)(ANNLAYER() _q)
{
    printf("neuron layer:\n");
    printf("    num inputs  : %u\n", _q->num_inputs);
    printf("    num outputs : %u\n", _q->num_nodes);
    unsigned int i;
    for (i=0; i<_q->num_nodes; i++)
        NODE(_print)(_q->nodes[i]);
}

void ANNLAYER(_evaluate)(ANNLAYER() _q)
{
    unsigned int i;
    for (i=0; i<_q->num_nodes; i++)
        NODE(_evaluate)(_q->nodes[i]);
}

// TODO: update ANNLAYER(_train) to work for both input and output layers
void ANNLAYER(_train)(ANNLAYER() _q,
                      T * _d,
                      float _eta)
{
}

