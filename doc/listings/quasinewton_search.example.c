// file: doc/listings/quasinewton_search.example.c
#include <liquid/liquid.h>

int main() {
    unsigned int num_parameters = 8;    // search dimensionality
    unsigned int num_iterations = 100;  // number of iterations to run
    float target_utility = 0.01f;       // target utility

    float optimum_vect[num_parameters];

    // ...

    // create quasinewton_search object
    quasinewton_search q = quasinewton_search_create(NULL,
                                                     optimum_vect,
                                                     num_parameters,
                                                     &rosenbrock,
                                                     LIQUID_OPTIM_MINIMIZE);

    // execute batch search
    quasinewton_search_execute(q, num_iterations, target_utility);

    // execute search one iteration at a time
    unsigned int i;
    for (i=0; i<num_iterations; i++)
        quasinewton_search_step(q);

    // clean it up
    quasinewton_search_destroy(q);
}
