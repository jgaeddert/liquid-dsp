//
// ann_layer_example.c
//
// artificial neural network (ann) layer example
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

#define OUTPUT_FILENAME "ann_layer_example.m"

int main() {
    // options
    unsigned int num_inputs = 3;    // number of inputs
    unsigned int num_outputs = 2;   // number of outputs

    // derived values
    unsigned int num_weights = (num_inputs+1)*num_outputs;

    float w[num_weights];           // weights vector
    float x[num_inputs];            // input vector
    float y[num_outputs];           // output vector

    unsigned int i;

    // initialize weights
    for (i=0; i<num_weights; i++)
        w[i] = 0.1f*(float)i;

    // initialize inputs
    for (i=0; i<num_inputs; i++)
        x[i] = randnf();

    // create the layer
    annlayer q = annlayer_create(w,x,y,num_inputs,num_outputs,0,1.0f);

    // evaluate
    annlayer_evaluate(q);

    // print
    annlayer_print(q);

    printf("[");
    for (i=0; i<num_inputs; i++)
        printf("%12.8f",x[i]);
    printf("] > [");
    for (i=0; i<num_outputs; i++)
        printf("%12.8f",y[i]);
    printf("]\n");
    
    return 0;

    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");


    // plot results
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"semilogy(e);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('training epoch');\n");
    fprintf(fid,"ylabel('error');\n");
    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    // clean up allocated objects
    annlayer_destroy(q);

    printf("done.\n");
    return 0;
}

