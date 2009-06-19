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
// Artificial neural network
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "liquid.internal.h"

#define ANN(name)       LIQUID_CONCAT(ann,name)
#define NODE(name)      LIQUID_CONCAT(node,name)
#define DOTPROD(name)   LIQUID_CONCAT(dotprod_rrrf,name)
#define T               float

#define LIQUID_ANN_MAX_NETWORK_SIZE 1024

#define DEBUG_ANN 0

struct ANN(_s) {
    // weights
    T * w;      // weights vector
    unsigned int num_weights;

    // network structure
    unsigned int * structure;
    unsigned int num_inputs;
    unsigned int num_outputs;
    unsigned int num_layers;
    unsigned int num_nodes;

    NODE() * nodes;
    T * y_hat;  // 

    // activation function (derivative) pointer
    T(*activation_func)(T);
    T(*d_activation_func)(T);

    // backpropagation
    T * dw;     // gradient weights vector [num_weights]
    //T * e;      // output node error vector [num_nodes]

};

// Creates a network
ANN() ANN(_create)(
        unsigned int * _structure,
        unsigned int _num_layers)
{
    unsigned int i;
    if (_num_layers < 2) {
        printf("error: ann_create(), must have at least 2 layers\n");
        exit(1);
    }

    for (i=0; i<_num_layers; i++) {
        if (_structure[i] < 1) {
            printf("error: ann_create(), layer %u has no nodes\n", i);
            exit(1);
        }
    }

    ann q;
    q = (ANN()) malloc( sizeof(struct ANN(_s)) );

    // Initialize structural variables
    q->num_layers = _num_layers;
    q->structure = (unsigned int*) malloc( sizeof(unsigned int)*_num_layers );
    for (i=0; i<q->num_layers; i++)
        q->structure[i] = _structure[i];
    q->num_inputs = q->structure[0];
    q->num_outputs = q->structure[q->num_layers-1];

    // Initialize weights
    q->num_weights = 2 * (q->structure[0]);
    q->num_nodes = q->structure[0];
    for (i=1; i<q->num_layers; i++) {
        q->num_weights += (q->structure[i-1]+1) * q->structure[i];
        q->num_nodes += q->structure[i];
    }

    if (q->num_weights > LIQUID_ANN_MAX_NETWORK_SIZE) {
        printf("error: ann_create(), network size exceeded\n");
        exit(1);
    }

    // allocate memory for weights, buffers
    q->w  = (T*) malloc( (q->num_weights)*sizeof(T) );
    q->dw = (T*) malloc( (q->num_weights)*sizeof(T) );
    q->y_hat = (T*) malloc( (q->num_nodes+q->num_inputs)*sizeof(T) );

    for (i=0; i<q->num_weights; i++) {
        q->w[i] = ((i%2)==0) ? 1.0 : -1.0;
        q->w[i] = i;
        q->dw[i] = 0.0f;
    }

    // create nodes
    q->nodes = (NODE()*) malloc((q->num_nodes)*sizeof(NODE()));
    unsigned int nw = 0;
    unsigned int nx = 0;
    unsigned int ny = q->num_inputs;
    unsigned int j, n=0;
    for (i=0; i<q->num_layers; i++) {
        printf("layer %3u\n", i);
        for (j=0; j<q->structure[i]; j++) {
            printf("  [%3u] nw : %3u, nx : %3u, inputs : %3u, ny : %3u\n",
                    n, nw, nx, (i==0) ? 1 : q->structure[i-1], ny);
            q->nodes[n] = NODE(_create)(q->w + nw,
                                        q->y_hat + nx,
                                        q->y_hat + ny,
                                        (i==0) ? 1 : q->structure[i-1],
                                        0,
                                        1.0f);
            nw += (i==0) ? 2 : q->structure[i-1]+1;
            nx += (i==0) ? 1 : 0;
            ny++;
            n++;
        }
        nx += (i==0) ? 0 : q->structure[i-1];
    }

    return q;
}

void ANN(_destroy)(ANN() _q)
{
    free(_q->structure);
    free(_q->w);
    free(_q->dw);
    free(_q->y_hat);

    // destroy node objects
    unsigned int i;
    for (i=0; i<_q->num_nodes; i++)
        NODE(_destroy)(_q->nodes[i]);
    free(_q->nodes);

    free(_q);
}

// Prints network
void ANN(_print)(ANN() _q)
{
    unsigned int i;
    printf("percepton network : [");
    for (i=0; i<_q->num_layers; i++)
        printf("%3u",_q->structure[i]);
    printf("]\n");
    printf("    num weights : %u\n", _q->num_weights);
    printf("    num inputs  : %u\n", _q->num_inputs);
    printf("    num outputs : %u\n", _q->num_outputs);
    printf("    num nodes   : %u\n", _q->num_nodes);
    printf("    num layers  : %u\n", _q->num_layers);

    for (i=0; i<_q->num_weights; i++)
        printf("  w[%4u] = %12.8f\n", i, _q->w[i]);

    for (i=0; i<_q->num_nodes; i++)
        node_print(_q->nodes[i]);
}

// Evaluates the network _q at _input and stores the result in _output
void ANN(_evaluate)(ANN() _q, T * _x, T * _y)
{
    // copy input elements to head of buffer
    memmove(_q->y_hat, _x, (_q->num_inputs)*sizeof(T));

#if 0
    unsigned int i,j,t,n=0;
    printf("\n\n\n");
    for (i=0; i<_q->num_layers; i++) {
        printf("------------------\n");
        printf("evaluating layer %3u\n", i);
        for (j=0; j<_q->structure[i]; j++) {
            NODE(_evaluate)(_q->nodes[n]);
            NODE(_print)(_q->nodes[n]);
            n++;
        }
    }
#else
    unsigned int i;
    for (i=0; i<_q->num_nodes; i++) {
        NODE(_evaluate)(_q->nodes[i]);
    }
#endif

    // copy output
    memmove(_y, &_q->y_hat[_q->num_nodes + _q->num_inputs - _q->num_outputs], (_q->num_outputs)*sizeof(float));
}

// train network
//
// _q       :   network object
// _x       :   input training pattern array [_n x nx]
// _y       :   output training pattern array [_n x ny]
// _n       :   number of training patterns
// _emax    :   maximum error tolerance
// _nmax    :   maximum number of iterations
void ANN(_train)(ANN() _q,
                 T * _x,
                 T * _y,
                 unsigned int _n,
                 T _emax,
                 unsigned int _nmax)
{
    // TODO: implement ANN(_train) method

    // for now, just compute error
    T y_hat[_q->num_outputs];
    float d, e;
    float rmse=0.0f;
    unsigned int i, j;
    for (i=0; i<_n; i++) {
        // evaluate network
        ANN(_evaluate)(_q, &_x[i*(_q->num_inputs)], y_hat);

        // compute error
        e = 0.0f;
        for (j=0; j<_q->num_outputs; j++) {
            printf("y[%3u] = %12.8f (expected %12.8f)\n",
                    i, y_hat[j],_y[i*(_q->num_outputs)+j]);
            d = y_hat[j] - _y[i*(_q->num_outputs) + j];
            e += d*d;
        }
        rmse += e / (_q->num_outputs);
    }

    rmse = sqrtf(rmse / _n);

    printf("rmse : %12.8f\n", rmse);
}

// Train using backpropagation
// _q       :   network object
// _x       :   input training pattern array [_n x nx]
// _y       :   output training pattern array [_n x ny]
void ANN(_train_bp)(ANN() _q,
                    T * _x,
                    T * _y)
{
    //unsigned int i;

    // evaluate network
    T y_hat[_q->num_outputs];
    ANN(_evaluate)(_q, _x, y_hat);

    // compute node error
    // starting at output layer and working backwards
    //for (i=_q->
}


