// 
// ann_maxnet_example.c
//
// artificial neural network (ann) maxnet example
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "liquid.internal.h"

#define OUTPUT_FILENAME "ann_maxnet_example.m"

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

    // classification
    unsigned int class[4] = {0, 1, 1, 0};

    // create network and initialize weights randomly
    maxnet q = maxnet_create(2, structure, 3);
    maxnet_print(q);

    float x_test[2] = {1,1};
    float y_test[2];
    unsigned int c_test;
    maxnet_evaluate(q,x_test,y_test,&c_test);

    unsigned int i;
    for (i=0; i<2; i++) {
        printf("  %3u : %12.8f", i, y_test[i]);
        if (i == c_test)
            printf(" *");
        printf("\n");
    }

    FILE* fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n",OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");

    unsigned int num_trials=2000;
    unsigned int n;
    for (n=0; n<num_trials; n++) {
        // compute error
        float rmse = maxnet_compute_rmse(q,x,class,4);
        fprintf(fid,"e(%6u) = %16.8e;\n", n+1, rmse);
        if ((n%100)==0) {
            printf("epoch %6u : %12.8f\n", n, rmse);

            //for (i=0; i<4; i++)
            //    maxnet_evaluate(q,&x[i*2],y_test,&c_test);
        }

        for (i=0; i<4; i++)
            maxnet_train(q,&x[i*2],class[i]);
    }

    fprintf(fid,"\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"semilogy(e);\n");
    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);
 
    maxnet_destroy(q);

    printf("done.\n");
    return 0;
}

