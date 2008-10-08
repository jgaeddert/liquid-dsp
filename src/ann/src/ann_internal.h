//
// Artificial neural network, internal header
//

#ifndef __LIQUID_ANN_INTERNAL_H__
#define __LIQUID_ANN_INTERNAL_H__

#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

#define LIQUID_ANN_MAX_NETWORK_SIZE 128

typedef float(*activation_function_prototype)(float);

typedef struct ann_neuron_s * ann_neuron;
struct ann_neuron_s {
    float * weights;
    unsigned int num_inputs;
    float * input_buffer;
};

float ann_neuron_evaluate(ann_neuron _p, float * _x);
void ann_neuron_print(ann_neuron _p);

typedef struct ann_layer_s * ann_layer;
struct ann_layer_s {
    ann_neuron * nodes;
    unsigned int num_nodes;
    unsigned int num_inputs;
    bool input_layer;
    bool output_layer;
    activation_function_prototype activation_function;
};

void ann_layer_print(ann_layer _n);
void ann_layer_evaluate(ann_layer _ni, float *_x, float *_y);

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

    float * buffer;
    unsigned int buffer_len;

    float * train_x;
    float * train_y;
    unsigned int num_patterns;
};

//ann ann_create(unsigned int _num_layers, ...);
ann ann_create(unsigned int _num_layers, unsigned int * _structure);
void ann_destroy(ann _n);
void ann_link_nodes(ann _n);
void ann_print(ann _n);

#endif // __LIQUID_ANN_INTERNAL_H__

