//
// ga_search_example.c
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "ga_search_example.m"

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
        u += (v-0.5f)*(v-0.5f);
    }

    return u;
}

int main() {
    unsigned int num_parameters = 8;    // dimensionality of search (minimum 1)
    unsigned int bits_per_parameter = 12;
    unsigned int num_iterations = 1000; // number of iterations to run

    unsigned int i;
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
                                    NULL,
                                    prototype,
                                    LIQUID_OPTIM_MINIMIZE);
    ga_search_print(ga);

    // execute search
    //optimum_utility = ga_search_run(ga, num_iterations, -1e-6f);

    // execute search one iteration at a time
    fprintf(fid,"u = zeros(1,%u);\n", num_iterations);
    for (i=0; i<num_iterations; i++) {
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
