// file: doc/listings/qnsearch.example.c
#include <liquid/liquid.h>

int main() {
    unsigned int num_parameters = 8;    // search dimensionality
    unsigned int num_iterations = 100;  // number of iterations to run
    float target_utility = 0.01f;       // target utility

    float optimum_vect[num_parameters];

    // ...

    // create qnsearch object
    qnsearch q = qnsearch_create(NULL,
                                 optimum_vect,
                                 num_parameters,
                                 &rosenbrock,
                                 LIQUID_OPTIM_MINIMIZE);

    // execute batch search
    qnsearch_execute(q, num_iterations, target_utility);

    // execute search one iteration at a time
    unsigned int i;
    for (i=0; i<num_iterations; i++)
        qnsearch_step(q);

    // clean it up
    qnsearch_destroy(q);
}
