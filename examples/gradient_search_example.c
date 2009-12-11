//
// gradient_search_example.c
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "gradient_search_example.m"

// N-dimensional rosenbrock function
float rosenbrock(void * _x, float* _opt, unsigned int _len);

int main() {
    unsigned int num_parameters = 4;
    float optimum_vect[num_parameters];
    unsigned int i;
    for (i=0; i<num_parameters; i++)
        optimum_vect[i] = 0.0f;

    float optimum_utility;

    // try to create gradient_search object
    gradient_search * gs;
    gs = gradient_search_create(
        NULL, optimum_vect, num_parameters, &rosenbrock, LIQUID_OPTIM_MINIMIZE);

    // execute search
    optimum_utility = gradient_search_run(gs, 10000, -1e-6f);

    // print results
    printf("\n");
    gradient_search_print(gs);

    // test results, optimum at [1, 1, 1, ... 1];
    /*
    TS_ASSERT_DELTA(optimum_utility, 0.0f, 0.001f);
    for (i=0;i <num_parameters; i++)
        TS_ASSERT_DELTA(optimum_vect[i], 1.0f, 0.01f);
    */

    free_gradient_search(gs);

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

