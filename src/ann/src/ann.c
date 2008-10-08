//
// Artificial neural network
//

#include <assert.h>
#include <stdio.h>
#include <math.h>

#include "ann_internal.h"

// Creates a network
ann ann_create(
        unsigned int _num_layers,
        unsigned int* _structure)
{
    if (_num_layers < 2) {
        perror("error: ann_create(), must have at least 2 layers\n");
        return NULL;
    }

    ann pn;
    pn = (ann) malloc( sizeof(ann) );

    // Initialize structural variables
    unsigned int i;
    pn->num_layers = _num_layers;
    pn->structure = (unsigned int*) malloc( sizeof(unsigned int)*_num_layers );
    for (i=0; i<pn->num_layers; i++)
        pn->structure[i] = _structure[i];
    pn->num_inputs = pn->structure[0];
    pn->num_outputs = pn->structure[pn->num_layers-1];

    // Initialize weights
    pn->num_weights = 2 * (pn->structure[0]);
    for (i=1; i<pn->num_layers; i++)
        pn->num_weights += (pn->structure[i-1]+1) * pn->structure[i];
    if (pn->num_weights > LIQUID_ANN_MAX_NETWORK_SIZE) {
        perror("error: ann_create(), network size exceeded\n");
        return NULL;
    }
    pn->weights = (float*) calloc( sizeof(float), pn->num_weights );
    if (!pn->weights) {
        perror("error: ann_create(), could not allocate memory for weights\n");
        return NULL;
    }

    // Create nodes
    pn->num_nodes = 0;
    for (i=0; i<pn->num_layers; i++)
        pn->num_nodes += pn->structure[i];
    pn->nodes = (ann_neuron*) malloc( sizeof(ann_neuron)*pn->num_nodes );

    // Create buffer
    pn->buffer_len = pn->num_nodes;
    pn->buffer = (float*) calloc( sizeof(float), pn->buffer_len );

    // Create layers
    pn->layers = (ann_layer*) malloc( sizeof(ann_layer)*pn->num_layers );
    for (i=0; i<pn->num_layers; i++) {
        pn->layers[i]->num_nodes = pn->structure[i];
        pn->layers[i]->input_layer = (i==0);
        pn->layers[i]->output_layer = (i==pn->num_layers-1);
    }

    pn->layers[0]->num_inputs = pn->num_inputs;
    for (i=1; i<pn->num_layers; i++)
        pn->layers[i]->num_inputs = pn->structure[i-1];

    // link the nodes
    ann_link_nodes(pn);

    // Set training pattern variables
    pn->train_x = NULL;
    pn->train_y = NULL;
    pn->num_patterns = 0;
    return pn;
}

void free_ann(ann _pn)
{
    free(_pn->structure);
    free(_pn->weights);
    free(_pn->layers);
    free(_pn->nodes);
    free(_pn->buffer);
    free(_pn);
}

// Prints network
void ann_print(ann _pn)
{
    unsigned int i;
    printf("percepton network:\n");
    for (i=0; i<_pn->num_layers; i++)
        ann_layer_print(_pn->layers[i]);
}

// Evaluates the network _pn at _input and stores the result in _output
void ann_evaluate(
        ann _pn,
        float* _input,
        float* _output)
{
    unsigned int i;
    unsigned int b=0;   // buffer index
    float* buffer_in;   // input buffer pointer
    float* buffer_out;  // output buffer pointer
    for (i=0; i<_pn->num_layers; i++) {
        buffer_in = (i==0) ? _input : &_pn->buffer[b];
        b += _pn->structure[b];
        buffer_out = (i==_pn->num_layers-1) ? _output : &_pn->buffer[b];

        ann_layer_evaluate(_pn->layers[i], buffer_in, buffer_out);
    }
}


// links nodes together after a network has been created
void ann_link_nodes(ann _pn)
{
    //
    unsigned int i, j;
    unsigned int n=0;   // nodes index
    unsigned int w=0;   // weights index

    for (i=0; i<_pn->num_layers; i++) {
        _pn->layers[i]->nodes = &_pn->nodes[n];
        _pn->layers[i]->num_nodes = _pn->structure[i];
        _pn->layers[i]->activation_function = &tanhf;
        for (j=0; j<_pn->layers[i]->num_nodes; j++) {
            _pn->layers[i]->nodes[j]->num_inputs = 
                _pn->layers[i]->input_layer ? 1 : _pn->layers[i]->num_inputs;
            _pn->layers[i]->nodes[j]->weights = &(_pn->weights[w]);
            w += _pn->layers[i]->nodes[j]->num_inputs + 1;
            //_pn->layers[i]->nodes[j]->activation_function = &tanhf;
        }
        n += _pn->structure[i];
    }

    // Assert that we aren't pointing to unbounded memory
    assert( n==_pn->num_nodes );
    assert( w==_pn->num_weights );
}

