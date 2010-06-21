//
// ga_search_example.c
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "ga_search_example.m"

// example data
typedef struct {
    float * v;      // parameter vector
    unsigned int n; // vector length
} exampledata;

// utility callback function
float utility_callback(void * _userdata, chromosome _c)
{
    exampledata * q = (exampledata*) _userdata;

    unsigned int i;
    for (i=0; i<q->n; i++)
        q->v[i] = chromosome_valuef(_c,i);

    return rosenbrock(NULL, q->v, q->n);
}

int main() {
    unsigned int num_parameters = 8;    // dimensionality of search (minimum 2)
    unsigned int bits_per_parameter = 3;
    unsigned int num_iterations = 4000; // number of iterations to run

    float optimum_vect[num_parameters];
    unsigned int i;
    for (i=0; i<num_parameters; i++)
        optimum_vect[i] = 0.0f;

    // create example user data
    exampledata u = {optimum_vect, num_parameters};

    float optimum_utility;

    // open output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");

    // create prototype chromosome
    chromosome prototype = chromosome_create_basic(num_parameters, bits_per_parameter);

    // create ga_search object
    ga_search ga = ga_search_create(&utility_callback,
                                    (void*)&u,
                                    prototype,
                                    LIQUID_OPTIM_MINIMIZE);
    ga_search_print(ga);

    // execute search
    //optimum_utility = ga_search_run(ga, num_iterations, -1e-6f);

    // execute search one iteration at a time
    fprintf(fid,"u = zeros(1,%u);\n", num_iterations);
    for (i=0; i<num_iterations; i++) {
        //optimum_utility = rosenbrock(NULL,optimum_vect,num_parameters);

        ga_search_evolve(ga);

        ga_search_getopt(ga, prototype, &optimum_utility);
        fprintf(fid,"u(%3u) = %12.4e;\n", i+1, optimum_utility);

        if (((i+1)%100)==0) {
            //ga_search_print(ga);
            printf("%4u : %16.8f\n", i, optimum_utility);
        }
    }

    // print results
    printf("\n");
    ga_search_print(ga);

    printf("optimum utility : %12.8f\n");
    chromosome_printf(prototype);

    fprintf(fid,"figure;\n");
    fprintf(fid,"semilogy(u);\n");
    fprintf(fid,"xlabel('iteration');\n");
    fprintf(fid,"ylabel('utility');\n");
    fprintf(fid,"title('gradient search results');\n");
    fprintf(fid,"grid on;\n");
    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    // test results, optimum at [1, 1, 1, ... 1];

    chromosome_destroy(prototype);
    ga_search_destroy(ga);

    return 0;
}
