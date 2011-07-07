//
// gradsearch_example.c
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "gradsearch_example.m"

int main() {
    unsigned int num_parameters = 8;    // dimensionality of search (minimum 2)
    unsigned int num_iterations = 4000; // number of iterations to run

    float optimum_vect[num_parameters];
    unsigned int i;
    for (i=0; i<num_parameters; i++)
        optimum_vect[i] = 0.0f;

    float optimum_utility;

    // open output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");

    // create gradsearch object
    gradsearch gs = gradsearch_create(NULL,
                                      optimum_vect,
                                      num_parameters,
                                      &rosenbrock,
                                      LIQUID_OPTIM_MINIMIZE,
                                      NULL);

    // execute search
    //optimum_utility = gradsearch_run(gs, num_iterations, -1e-6f);

    // execute search one iteration at a time
    fprintf(fid,"u = zeros(1,%u);\n", num_iterations);
    for (i=0; i<num_iterations; i++) {
        optimum_utility = rosenbrock(NULL,optimum_vect,num_parameters);
        fprintf(fid,"u(%3u) = %12.4e;\n", i+1, optimum_utility);

        gradsearch_step(gs);

        if (((i+1)%100)==0)
            gradsearch_print(gs);
    }

    // print results
    printf("\n");
    gradsearch_print(gs);

    fprintf(fid,"figure;\n");
    fprintf(fid,"semilogy(u);\n");
    fprintf(fid,"xlabel('iteration');\n");
    fprintf(fid,"ylabel('utility');\n");
    fprintf(fid,"title('gradient search results');\n");
    fprintf(fid,"grid on;\n");
    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    // test results, optimum at [1, 1, 1, ... 1];

    gradsearch_destroy(gs);

    return 0;
}
