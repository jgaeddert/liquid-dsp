//
// Artificial neural network
//

#include <assert.h>
#include <stdio.h>

#include "ann_internal.h"

float ann_neuron_evaluate(ann_neuron _p, float* _input)
{
    float v;

    // 
    //v = dot_product(_input, _p->weights, _p->num_inputs);
    unsigned int i;
    v = 0.0f;
    for (i=0; i<_p->num_inputs; i++)
        v += _input[i] * _p->weights[i];

    v += _p->weights[_p->num_inputs];

    return v;
}

void ann_neuron_print(ann_neuron _p)
{
    unsigned int i;
    for (i=0; i<_p->num_inputs+1; i++)
        printf("    w[%u] = %0.3f\n", i, _p->weights[i]);
}


