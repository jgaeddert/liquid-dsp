const char __docstr__[] = "Demonstrate quasi-Newton search method.";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "qnsearch_example.m",'o', "output filename", NULL);
    liquid_argparse_add(unsigned, num_parameters,    8, 'p', "dimensionality of search (minimum 2)", NULL);
    liquid_argparse_add(unsigned, num_iterations, 4000, 'n', "number of iterations to run", NULL);
    liquid_argparse_parse(argc,argv);

    LIQUID_VLA(float, optimum_vect, num_parameters);
    unsigned int i;
    for (i=0; i<num_parameters; i++)
        optimum_vect[i] = 0.0f;

    float optimum_utility;

    // open output file
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");

    // create qnsearch object
    qnsearch gs = qnsearch_create(
        NULL, optimum_vect, num_parameters, &liquid_rosenbrock, LIQUID_OPTIM_MINIMIZE);

    // execute search
    //optimum_utility = qnsearch_run(gs, num_iterations, -1e-6f);

    // execute search one iteration at a time
    fprintf(fid,"u = zeros(1,%u);\n", num_iterations);
    for (i=0; i<num_iterations; i++) {
        optimum_utility = liquid_rosenbrock(NULL,optimum_vect,num_parameters);
        fprintf(fid,"u(%3u) = %12.4e;\n", i+1, optimum_utility);

        qnsearch_step(gs);

        if (((i+1)%100)==0)
            qnsearch_print(gs);
    }

    // print results
    printf("\n");
    qnsearch_print(gs);

    fprintf(fid,"figure;\n");
    fprintf(fid,"semilogy(u);\n");
    fprintf(fid,"xlabel('iteration');\n");
    fprintf(fid,"ylabel('utility');\n");
    fprintf(fid,"title('quasinewton search results');\n");
    fprintf(fid,"grid on;\n");
    fclose(fid);
    printf("results written to %s.\n", filename);

    // test results, optimum at [1, 1, 1, ... 1];

    qnsearch_destroy(gs);

    return 0;
}
