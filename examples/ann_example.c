//
// ann_example.c
//
// Artificial neural network (ann) example. This example demonstrates
// the functionality of the ann module by training a simple network
// to learn the output of a continuous function.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "liquid.experimental.h"

#define OUTPUT_FILENAME "ann_example.m"

int main() {
    // set random seed
    srand(time(NULL));

    // options
    unsigned int num_patterns = 250;

    // fixed values
    unsigned int num_inputs = 2;
    unsigned int num_outputs = 2;

    // create network structure
    unsigned int structure[4] = {num_inputs, 6, 4, num_outputs};

    float x[num_inputs*num_patterns];   // input pattern
    float y[num_outputs*num_patterns];  // output pattern

    unsigned int i, j;
    for (i=0; i<num_patterns; i++) {
        float x0 = randnf();
        float x1 = randnf();

        // output functions
        float y0 = exp(-(x0*x0 + x1*x1));
        float y1 = tanhf(4.0f*(x0-x1)) * y0;

        x[2*i+0] = x0;
        x[2*i+1] = x1;

        y[2*i+0] = y0;
        y[2*i+1] = y1;
    }

    // create network and initialize weights randomly
    ann q = ann_create(structure, 4, LIQUID_ANN_AF_TANH,
                                     LIQUID_ANN_AF_LINEAR);
    ann_init_random_weights(q);

    float error_tolerance = 1e-3f;
    unsigned int max_trials = 2000;

    printf("training...\n");
#if 0
    //ann_train(q,x,y,num_patterns,error_tolerance,max_trials);
    ann_train_ga(q,x,y,num_patterns,error_tolerance,500);
#endif
    FILE* fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n",OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");

    float rmse;
    printf(" epoch :   rms error\n");
    printf(" ----- :   ---------------\n");
    for (i=0; i<max_trials; i++) {
        // run single training epoch
        for (j=0; j<num_patterns; j++)
            ann_train_bp(q,&x[j*num_inputs],&y[j*num_outputs]);

        // compute error
        rmse = ann_compute_rmse(q,x,y,num_patterns);

        // print error periodically
        if ( (i%100)==0 )
            printf("%6u : %16.8e;\n", i, rmse);

        // output error to file
        fprintf(fid,"rmse(%6u) = %16.8e;\n", i+1, rmse);

        // break if error is below tolerance
        if (rmse < error_tolerance) {
            printf("reached error limit\n");
            break;
        }
    }

    printf("done.\n");
    
    float x_test[2];
    float y_test[2];
    unsigned int n_test=21;
    float xmin = -2.0f;
    float xmax =  2.0f;
    float dx = (xmax - xmin) / (float)(n_test-1);

    fprintf(fid,"x0 = zeros(1,%3u);\n", n_test);
    for (i=0; i<n_test; i++)
        fprintf(fid,"x0(%3u) = %12.4e;\n", i+1, xmin +i*dx);
    
    fprintf(fid,"y0 = zeros(%3u,%3u);\n", n_test, n_test);
    fprintf(fid,"y1 = zeros(%3u,%3u);\n", n_test, n_test);
    for (i=0; i<n_test; i++) {
        for (j=0; j<n_test; j++) {
            x_test[0] = xmin + i*dx;
            x_test[1] = xmin + j*dx;

            ann_evaluate(q,x_test,y_test);

            fprintf(fid,"y0(%3u,%3u) = %12.4e;\n",i+1,j+1,y_test[0]);
            fprintf(fid,"y1(%3u,%3u) = %12.4e;\n",i+1,j+1,y_test[1]);
        }
    }

    fprintf(fid,"\n");
    fprintf(fid,"semilogy(rmse,'-','LineWidth',2)\n");
    fprintf(fid,"xlabel('training epoch');\n");
    fprintf(fid,"ylabel('rmse');\n");
    fprintf(fid,"grid on;\n");

    fprintf(fid,"\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"mesh(x0,x0,y0);\n");
    fprintf(fid,"xlabel('x_0');\n");
    fprintf(fid,"ylabel('x_1');\n");
    fprintf(fid,"zlabel('y_0');\n");

    fprintf(fid,"\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"mesh(x0,x0,y1);\n");
    fprintf(fid,"xlabel('x_0');\n");
    fprintf(fid,"ylabel('x_1');\n");
    fprintf(fid,"zlabel('y_1');\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    ann_destroy(q);

    printf("done.\n");
    return 0;
}

