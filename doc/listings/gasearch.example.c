// file: doc/listings/gasearch.example.c
#include <liquid/liquid.h>

// user-defined utility callback function
float myutility(void * _userdata, chromosome _c)
{
    // compute utility from chromosome
    float u = 0.0f;
    unsigned int i;
    for (i=0; i<chromosome_get_num_traits(_c); i++)
        u += chromosome_valuef(_c,i);
    return u;
}

int main() {
    unsigned int num_parameters = 8;        // dimensionality of search (minimum 1)
    unsigned int num_iterations = 100;      // number of iterations to run
    float target_utility = 0.01f;           // target utility

    unsigned int bits_per_parameter = 16;   // chromosome parameter resolution
    unsigned int population_size = 100;     // GA population size
    float mutation_rate = 0.10f;            // GA mutation rate

    // create prototype chromosome
    chromosome prototype = chromosome_create_basic(num_parameters, bits_per_parameter);

    // create gasearch object
    gasearch ga = gasearch_create_advanced(
                                   &myutility,
                                   NULL,
                                   prototype,
                                   LIQUID_OPTIM_MINIMIZE,
                                   population_size,
                                   mutation_rate);

    // execute batch search
    gasearch_run(ga, num_iterations, target_utility);

    // execute search one iteration at a time
    unsigned int i;
    for (i=0; i<num_iterations; i++)
        gasearch_evolve(ga);

    // clean up objects
    chromosome_destroy(prototype);
    gasearch_destroy(ga);
}
