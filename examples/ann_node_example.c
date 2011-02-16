//
// ann_node_example.c
//
// artificial neural network (ann) node example
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.experimental.h"

#define OUTPUT_FILENAME "ann_node_example.m"

int main() {
    // options
    unsigned int num_inputs = 8;    // number of inputs
    float eta = 1e-2f;              // training step size

    float w[num_inputs+1];          // weights vector
    float x[num_inputs];            // input vector
    float y[1];                     // output

    float y_hat = -0.5f;            // desired output

    unsigned int i;

    // initialize weights
    for (i=0; i<=num_inputs; i++)
        w[i] = 1.0f / num_inputs;

    // initialize inputs
    for (i=0; i<num_inputs; i++)
        x[i] = randnf();

    // create the node
    node q = node_create(w,x,y,num_inputs,0,1.0f);
    
    node_evaluate(q);
    node_print(q);

    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");

    // train the node to produce the desired output
    float e;
    printf("training...\n");
    for (i=0; i<64; i++) {
        node_evaluate(q);
        e = y_hat - y[0];
        fprintf(fid,"y(%3u) = %12.4e;\n", i+1, y[0]);
        fprintf(fid,"e(%3u) = %12.4e;\n", i+1, e*e);

        node_compute_bp_error(q,e);
        node_train(q,eta);
        //node_print(q);
    }
    node_print(q);
    printf("e : %12.4e;\n", e*e);

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
    node_destroy(q);

    printf("done.\n");
    return 0;
}

