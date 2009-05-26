//
// Artificial neural network
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "liquid.internal.h"

#define ANN(name)   LIQUID_CONCAT(ann,name)
#define T           float

#define LIQUID_ANN_MAX_NETWORK_SIZE 1024

struct ANN(_s) {
    // weights
    T * w;
    T * dw;
    unsigned int num_weights;

    // number of inputs
    unsigned int num_inputs;
    unsigned int num_outputs;
    unsigned int num_layers;
    unsigned int num_nodes;

    // network structure
    unsigned int * structure;
};

// Creates a network
ANN() ANN(_create)(
        unsigned int * _structure,
        unsigned int _num_layers)
{
    if (_num_layers < 2) {
        perror("error: ann_create(), must have at least 2 layers\n");
        return NULL;
    }

    ann q;
    q = (ANN()) malloc( sizeof(struct ANN(_s)) );

    // Initialize structural variables
    unsigned int i;
    q->num_layers = _num_layers;
    q->structure = (unsigned int*) malloc( sizeof(unsigned int)*_num_layers );
    for (i=0; i<q->num_layers; i++)
        q->structure[i] = _structure[i];
    q->num_inputs = q->structure[0];
    q->num_outputs = q->structure[q->num_layers-1];

    // Initialize weights
    q->num_weights = 2 * (q->structure[0]);
    for (i=1; i<q->num_layers; i++)
        q->num_weights += (q->structure[i-1]+1) * q->structure[i];
    if (q->num_weights > LIQUID_ANN_MAX_NETWORK_SIZE) {
        perror("error: ann_create(), network size exceeded\n");
        return NULL;
    }
    q->w = (T*) malloc( (q->num_weights)*sizeof(T) );
    if (!q->w) {
        perror("error: ann_create(), could not allocate memory for weights\n");
        return NULL;
    }

    return q;
}

void ANN(_destroy)(ANN() _q)
{
    free(_q->structure);
    free(_q->w);
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
}

// Evaluates the network _q at _input and stores the result in _output
void ANN(_evaluate)(ANN() _q, T * _x, T * _y)
{
}

