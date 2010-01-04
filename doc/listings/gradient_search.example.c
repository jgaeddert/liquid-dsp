#include <liquid/liquid.h>
// ...
{
    unsigned int num_parameters = 8;    // search dimensionality
    unsigned int num_iterations = 100;  // number of iterations to run
    float target_utility = 0.01f;       // target utility

    float optimum_vect[num_parameters];

    // ...

    // create gradient_search object
    gradient_search gs = gradient_search_create(NULL,
                                                optimum_vect,
                                                num_parameters,
                                                &rosenbrock,
                                                LIQUID_OPTIM_MINIMIZE);

    // execute batch search
    gradient_search_execute(gs, num_iterations, target_utility);

    // execute search one iteration at a time
    for (i=0; i<num_iterations; i++)
        gradient_search_step(gs);

    // clean it up
    gradient_search_destroy(gs);
}
