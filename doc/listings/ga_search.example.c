#include <liquid/liquid.h>

// utility callback function
float utility_callback(void * _userdata, chromosome _c)
{
    float u;
    // compute utility from chromosome
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

    // create ga_search object
    ga_search ga = ga_search_create_advanced(
                                    &utility_callback,
                                    NULL,
                                    prototype,
                                    LIQUID_OPTIM_MINIMIZE,
                                    population_size,
                                    mutation_rate);

    // execute batch search
    ga_search_run(ga, num_iterations, target_utility);

    // execute search one iteration at a time
    unsigned int i;
    for (i=0; i<num_iterations; i++)
        ga_search_evolve(ga);

    // clean up objects
    chromosome_destroy(prototype);
    ga_search_destroy(ga);
}
