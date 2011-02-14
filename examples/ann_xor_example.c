//
// ann_xor_example.c
//
// Artificial neural network (ann) eXclusive OR example. This
// example demonstrates the functionality of the ann  module by
// training a simple network to learn the output of an exclusive
// or (xor) circuit:
//      x   y   |   z
//      --------+-----
//      0   0   |   0
//      0   1   |   1
//      1   0   |   1
//      1   1   |   0
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "liquid.experimental.h"

#define OUTPUT_FILENAME "ann_xor_example.m"

int main() {
    // set random seed
    srand(time(NULL));

    // create network structure:
    //      2 inputs, 4 hidden neurons, 1 output
    unsigned int structure[3] = {2, 4, 1};

    // binary input sequence
    float x[8] = {-1,-1,    // 0 0
                  -1, 1,    // 0 1
                   1,-1,    // 1 0
                   1, 1     // 1 1
    };

    // binary output sequence (exclusive or)
    float y[4] = {-1, 1, 1, -1};
    float y_hat;

    // create network and initialize weights randomly
    ann q = ann_create(structure, 3, LIQUID_ANN_AF_TANH,
                                     LIQUID_ANN_AF_TANH);
    ann_init_random_weights(q);

    unsigned int i;
    // evaluate network
    for (i=0; i<4; i++) {
        ann_evaluate(q,&x[2*i],&y_hat);
        printf("%6.3f %6.3f > %6.3f (%12.8f)\n",
                x[2*i+0], x[2*i+1], y[i], y_hat);
    }

    unsigned int num_training_patterns = 4;
    float error_tolerance = 1e-3f;
    unsigned int max_trials = 2000;

    printf("training...\n");
#if 0
    //ann_train(q,x,y,num_training_patterns,error_tolerance,max_trials);
    ann_train_ga(q,x,y,num_training_patterns,error_tolerance,max_trials);
#else
    FILE* fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n",OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");

    float rmse;
    unsigned int j;
    printf(" epoch :   rms error\n");
    printf(" ----- :   ---------------\n");
    for (i=0; i<max_trials; i++) {
        // run single training epoch
        for (j=0; j<num_training_patterns; j++)
            ann_train_bp(q,&x[2*j],&y[j]);

        // compute error
        rmse = ann_compute_rmse(q,x,y,num_training_patterns);

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
    
    fprintf(fid,"\n");
    fprintf(fid,"semilogy(rmse,'-','LineWidth',2)\n");
    fprintf(fid,"xlabel('training epoch');\n");
    fprintf(fid,"ylabel('rmse');\n");
    fprintf(fid,"grid on;\n");
    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);
#endif
    printf("done.\n");

    for (i=0; i<num_training_patterns; i++) {
        ann_evaluate(q,&x[2*i],&y_hat);
        //ann_print(q);
        printf("%6.3f %6.3f > %6.3f (%12.8f)\n",
                x[2*i+0], x[2*i+1], y[i], y_hat);
    }


    ann_destroy(q);

    printf("done.\n");
    return 0;
}

