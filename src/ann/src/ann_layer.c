//
// Artificial neural network
//

#include <assert.h>
#include <stdio.h>

#include "ann_internal.h"

void ann_layer_evaluate(
    ann_layer _pl,
    float* _input,
    float* _output)
{
    unsigned int i;

    if (_pl->input_layer) {
        for (i=0; i<_pl->num_nodes; i++)
            _output[i] = ann_neuron_evaluate( _pl->nodes[i], &_input[i]);
    } else {
        for (i=0; i<_pl->num_nodes; i++)
            _output[i] = ann_neuron_evaluate( _pl->nodes[i], _input);
    }

    // apply activation value if not in output layer
    if (!_pl->output_layer) {
        for (i=0; i<_pl->num_nodes; i++)
            _output[i] = _pl->activation_function(_output[i]);
    }
}

void ann_layer_print(ann_layer _pl)
{
    unsigned int i;
    printf("layer:\n");
    for (i=0; i<_pl->num_nodes; i++) {
        printf("  node %u:\n", i);
        ann_neuron_print(_pl->nodes[i]);
    }
}


