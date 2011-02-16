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

    // activation function (derivative) pointer
    T(*activation_func)(T);
    T(*d_activation_func)(T);

    // backpropagation
    T * dw;     // gradient weights vector [num_weights]
    T * y_hat;  // 
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

    q->w  = (T*) malloc( (q->num_weights)*sizeof(T) );
    q->dw = (T*) malloc( (q->num_weights)*sizeof(T) );
    q->y_hat = (T*) malloc( (q->num_nodes)*sizeof(T) );

    for (i=0; i<q->num_weights; i++) {
        q->w[i] = ((i%2)==0) ? 1.0 : -1.0;
        q->dw[i] = 0.0f;
    }

    return q;
}

void ANN(_destroy)(ANN() _q)
{
    free(_q->structure);
    free(_q->w);
    free(_q->dw);
    free(_q->y_hat);
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
}

// Evaluates the network _q at _input and stores the result in _output
void ANN(_evaluate)(ANN() _q, T * _x, T * _y)
{
    T v;

    unsigned int i;     // (hidden) layer index

    T w0, w1;
    // compute first layer outputs
    for (i=0; i<_q->num_inputs; i++) {
        w0 = _q->w[2*i+0];
        w1 = _q->w[2*i+1];
        _q->y_hat[i] = tanhf( _x[i]*w0 + w1 );
        //_q->y_hat[i] =  _x[i]*w0 + w1;

#if DEBUG_ANN
        printf("w[%3u] = %12.8f\n", 2*i+0, w0);
        printf("w[%3u] = %12.8f\n", 2*i+1, w1);
#endif
    }

    unsigned int j;     // layer sub-node index
    unsigned int k=0;   // node input index (y_hat)
    unsigned int n=2*_q->num_inputs;   // weight index
    unsigned int m=  _q->num_inputs;   // node output index (y_hat)

#if DEBUG_ANN
    unsigned int t;     // temporary counter
#endif
    // traverse each hidden layer
    for (i=1; i<_q->num_layers; i++) {

#if DEBUG_ANN
        printf("layer %u :\n", i);

        printf("  input:\n");
        for (t=0; t<_q->structure[i-1]; t++)
            printf("  y_hat[%3u] = %12.8f\n", k+t, _q->y_hat[k+t]);
        printf("\n");
#endif

        // traverse each node in this layer
        for (j=0; j<_q->structure[i]; j++) {
            // number of weights for this node (not including bias)
            unsigned int nw = _q->structure[i-1];

#if DEBUG_ANN
            printf("  k : %u\n", k);
            printf("  n : %u\n", n);
            printf("  m : %u\n", m);
            printf("  nw: %u\n", nw);

            for (t=0; t<nw; t++)
                printf("    w[%2u] %12.8f * y_hat[%2u] %12.8f\n", n+t, _q->w[n+t], k+t,_q->y_hat[k+t]);
            printf("    w[%2u] %12.8f\n", n+t, _q->w[n+t]);
#endif

            // compute 
            DOTPROD(_run)(&(_q->y_hat[k]), &(_q->w[n]), nw, &v);
#if DEBUG_ANN
            printf("  bias index : %u\n", n+nw);
#endif
            v += _q->w[n+nw]; // add bias

            if (i != _q->num_layers-1) {
                // activation function
                _q->y_hat[m] = tanhf(v);
            } else {
                // no activation function
                _q->y_hat[m] = v;
            }
#if DEBUG_ANN
            printf("  v = %12.8f\n", v);
#endif

            n += _q->structure[i-1] + 1;
            m++;
#if DEBUG_ANN
            printf("\n");
#endif
        }
        k += _q->structure[i-1];
    }

    // copy output
    memmove(_y, &_q->y_hat[_q->num_nodes - _q->num_outputs], (_q->num_outputs)*sizeof(float));
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


