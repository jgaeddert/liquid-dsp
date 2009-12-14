//
// quasinewton_search_example.c
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "quasinewton_search_example.m"

// N-dimensional rosenbrock function
float rosenbrock(void * _x, float* _opt, unsigned int _len);

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

    // create quasinewton_search object
    quasinewton_search gs = quasinewton_search_create(
        NULL, optimum_vect, num_parameters, &rosenbrock, LIQUID_OPTIM_MINIMIZE);

    // execute search
    //optimum_utility = quasinewton_search_run(gs, num_iterations, -1e-6f);

    // execute search one iteration at a time
    fprintf(fid,"u = zeros(1,%u);\n", num_iterations);
    for (i=0; i<num_iterations; i++) {
        optimum_utility = rosenbrock(NULL,optimum_vect,num_parameters);
        fprintf(fid,"u(%3u) = %12.4e;\n", i+1, optimum_utility);

        quasinewton_search_step(gs);

        if (((i+1)%100)==0)
            quasinewton_search_print(gs);
    }

    // print results
    printf("\n");
    quasinewton_search_print(gs);

    fprintf(fid,"figure;\n");
    fprintf(fid,"semilogy(u);\n");
    fprintf(fid,"xlabel('iteration');\n");
    fprintf(fid,"ylabel('utility');\n");
    fprintf(fid,"title('quasinewton search results');\n");
    fprintf(fid,"grid on;\n");
    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    // test results, optimum at [1, 1, 1, ... 1];

    quasinewton_search_destroy(gs);

    return 0;
}

// N-dimensional rosenbrock function
float rosenbrock(void * _x, float* _opt, unsigned int _len)
{
    if (_len < 2)
        return 0.0f;

    float u=0;
    unsigned int i;
    for (i=0; i<(_len-1); i++)
        u += powf(1-_opt[i],2) + 100*powf( _opt[i+1] - powf(_opt[i],2), 2);

    return u;
}

