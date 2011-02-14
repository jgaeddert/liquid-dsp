//
// ann_layer_example.c
//
// artificial neural network (ann) layer example
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.experimental.h"

#define OUTPUT_FILENAME "ann_layer_example.m"

int main() {
    // options
    unsigned int num_inputs = 3;    // number of inputs
    unsigned int num_outputs = 2;   // number of outputs
    float eta = 0.01f;

    // derived values
    unsigned int num_weights = (num_inputs+1)*num_outputs;

    float w[num_weights];           // weights vector
    float x[num_inputs];            // input vector
    float y[num_outputs];           // output vector

    float y_hat[num_outputs];       // expected output

    unsigned int i;

    // initialize weights
    for (i=0; i<num_weights; i++)
        w[i] = 0.1f*(float)i;

    // initialize inputs
    for (i=0; i<num_inputs; i++)
        x[i] = randnf();

    //
    for (i=0; i<num_outputs; i++)
        y_hat[i] = (i%2 ? 0.5f : -0.5f);

    // create the layer
    annlayer q = annlayer_create(w,x,y,num_inputs,num_outputs,0,0,LIQUID_ANN_AF_LINEAR,1.0f);

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
    
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");


    // train
    printf("*********************************\n");
    float error[num_outputs];

    for (i=0; i<num_outputs; i++)
        printf("%12.8f (expected %12.8f)\n", y[i], y_hat[i]);

    for (i=0; i<1; i++) {
        // evaluate the network
        annlayer_evaluate(q);

        // compute error
        unsigned int j;
        float rmse=0.0f;
        for (j=0; j<num_outputs; j++) {
            error[j] = y_hat[j] - y[j];
            rmse += error[j]*error[j];
        }

        fprintf(fid,"e(%3u) = %12.4e;\n", i+1, rmse);

        // update back-propagation error
        annlayer_compute_bp_error(q,error);

        // update weights (train)
        annlayer_train(q,eta);
    }

    //annlayer_print(q);

    for (i=0; i<num_outputs; i++)
        printf("%12.8f (expected %12.8f)\n", y[i], y_hat[i]);

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

