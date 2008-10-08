//
// Artificial neural network, internal header
//

#ifndef __LIQUID_ANN_INTERNAL_H__
#define __LIQUID_ANN_INTERNAL_H__

#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

typedef float(*activation_function)(float);

typedef struct ann_neuron_s * ann_neuron;
struct ann_neuron_s {
    float * weights;
    unsigned int num_inputs;
    float * input_buffer;
}

typedef ann_layer_s * ann_layer;
struct ann_layer_s {
    ann_neuron * nodes;
    unsigned int num_nodes;
    unsigned int num_inputs;
    bool input_layer;
    bool output_layer;
    activation_function f;
}

typedef struct ann_s * ann;
struct ann_s {
    unsigned int * structure;
    unsigned int num_inputs;
    unsigned int num_outputs;

    ann_neuron * nodes;
    unsigned int num_nodes;

    ann_layer * layers;
    unsigned int num_layers;

    float * weights;
    float * dw;
    unsigned int num_weights;
}

ann ann_create(unsigned int _num_layers, ...);
void ann_destroy(ann _n);

#endif // __LIQUID_ANN_INTERNAL_H__

