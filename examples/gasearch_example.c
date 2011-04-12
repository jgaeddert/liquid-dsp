//
// gasearch_example.c
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "gasearch_example.m"

// utility callback function
float utility_callback(void * _userdata, chromosome _c);

// peak callback function; value nearest {0.5, 0.5, 0.5, ...}
float peak_callback(void * _userdata, chromosome _c);

// rosenbrock callback function
float rosenbrock_callback(void * _userdata, chromosome _c);

int main() {
    unsigned int num_parameters = 8;        // dimensionality of search (minimum 1)
    unsigned int bits_per_parameter = 16;   // parameter resolution
    unsigned int num_iterations = 1000;     // number of iterations to run
    unsigned int population_size = 20;      // GA population size
    float mutation_rate = 0.10f;            // GA mutation rate

    unsigned int i;
    float optimum_utility;

    // open output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");

    // create prototype chromosome
    chromosome prototype = chromosome_create_basic(num_parameters, bits_per_parameter);

    // create gasearch object
    gasearch ga = gasearch_create_advanced(
                                    //&utility_callback,
                                    //&rosenbrock_callback,
                                    &peak_callback,
                                    NULL,
                                    prototype,
                                    LIQUID_OPTIM_MAXIMIZE,
                                    population_size,
                                    mutation_rate);
    gasearch_print(ga);

    // execute search
    //optimum_utility = gasearch_run(ga, num_iterations, -1e-6f);

    // execute search one iteration at a time
    fprintf(fid,"u = zeros(1,%u);\n", num_iterations);
    for (i=0; i<num_iterations; i++) {
        gasearch_evolve(ga);

        gasearch_getopt(ga, prototype, &optimum_utility);
        fprintf(fid,"u(%3u) = %12.4e;\n", i+1, optimum_utility);

        printf("%4u : %16.8f\n", i, optimum_utility);
    }

    // print results
    printf("\n");
    gasearch_print(ga);

    printf("optimum utility : %12.8f\n", optimum_utility);
    chromosome_printf(prototype);

    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(u);\n");
    fprintf(fid,"xlabel('iteration');\n");
    fprintf(fid,"ylabel('utility');\n");
    fprintf(fid,"title('gradient search results');\n");
    fprintf(fid,"grid on;\n");
    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    // test results, optimum at [1, 1, 1, ... 1];

    chromosome_destroy(prototype);
    gasearch_destroy(ga);

    return 0;
}

// utility callback function
float utility_callback(void * _userdata, chromosome _c)
{
    unsigned int n = chromosome_get_num_traits(_c);
    float v;    // chromosome value
    float u=0;  // total utility

    unsigned int i;
    for (i=0; i<n; i++) {
        // extract chromosome value
        v = chromosome_valuef(_c,i);

        // accumulate utility
        u += v;
    }

    return sqrtf(fabsf(u-1.0));
}

// peak callback function; value nearest {0.5, 0.5, 0.5, ...}
float peak_callback(void * _userdata, chromosome _c)
{
    unsigned int n = chromosome_get_num_traits(_c);
    float v[n]; // chromosome values

    unsigned int i;
    float u_global = 1.0f;
    float sig = 0.2f;
    for (i=0; i<n; i++) {
        // extract chromosome values
        v[i] = chromosome_valuef(_c,i);

        float e = v[i] - 0.5f;
        float u = exp(-e*e/(2*sig*sig));
        u_global *= u;
    }

    return u_global;
}


