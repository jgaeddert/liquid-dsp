//
// ann_bitpattern_example.c
//
// Artificial neural network (ann) bit pattern example.  This example
// demonstrates the functionality of the ann module by training a
// simple network to learn prime numbers.
//
//      n   :   b0  b1  b2  |   y
//      --------------------+------
//      0   :   0   0   0   |   1
//      1   :   0   0   1   |   1
//      2   :   0   1   0   |   1
//      3   :   0   1   1   |   1
//      4   :   1   0   0   |   0
//      5   :   1   0   1   |   1
//      6   :   1   1   0   |   0
//      7   :   1   1   1   |   1
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "liquid.experimental.h"

#define OUTPUT_FILENAME "ann_bitpattern_example.m"

// determines if the number _n is prime
int isprime(unsigned int _n);

int main() {
    // set random seed
    srand(time(NULL));

    // options
    unsigned int num_bits=5;
    unsigned int num_hidden_layers = 6;

    // derived values
    unsigned int M = 1<<num_bits;

    // network structure
    unsigned int structure[3] = {num_bits, num_hidden_layers, 1};

    // input, output sequences
    float x[M*num_bits];
    float y[M];
    unsigned int i, j;
    for (i=0; i<M; i++) {
        // binary sequence representation of 'i'
        for (j=0; j<num_bits; j++) {
            x[i*num_bits + j] = ((i>>(num_bits-j-1)) & 1) ? 1.0f : -1.0f;
        }
        
        y[i] = isprime(i) ? 1.0f : -1.0f;
        //y[i] = i % 2 ? 1.0f : -1.0f;
        //y[i] = i % 3 ? 1.0f : -1.0f;
    }

    // binary output
    float y_hat;

    // create network and initialize weights randomly
    ann q = ann_create(structure, 3, LIQUID_ANN_AF_TANH,
                                     LIQUID_ANN_AF_TANH);
    ann_init_random_weights(q);

    // evaluate network
    for (i=0; i<M; i++) {
        ann_evaluate(q, &x[num_bits*i], &y_hat);

        printf("  %4u :  ", i);
        for (j=0; j<num_bits; j++)
            printf("%6.3f ", x[num_bits*i+j]);
        printf(" > %6.3f (%12.8f)\n", y[i], y_hat);
    }

    unsigned int num_training_patterns = M;
    float error_tolerance = 0.01f;
    unsigned int max_trials = 1000*num_bits;

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
    printf(" epoch :   rms error\n");
    printf(" ----- :   ---------------\n");
    for (i=0; i<max_trials; i++) {

        // run single training epoch
        for (j=0; j<num_training_patterns; j++)
            ann_train_bp(q, &x[num_bits*j], &y[j]);

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
        ann_evaluate(q,&x[num_bits*i],&y_hat);
        //ann_print(q);

        printf("  %4u :  ", i);
        for (j=0; j<num_bits; j++)
            printf("%6.3f ", x[num_bits*i+j]);
        printf(" > %6.3f (%12.8f)\n", y[i], y_hat);
    }


    ann_destroy(q);

    printf("done.\n");
    return 0;
}

// cheap and dirty method to compute prime numbers
int isprime(unsigned int _n)
{
    unsigned int i;
    div_t d;
    for (i=2; i<_n; i++) {
        d = div(_n, i);
        if (d.rem == 0)
            return 0;
    }

    return 1;
}

