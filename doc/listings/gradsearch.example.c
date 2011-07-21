// file: doc/listings/gradsearch.example.c
#include <liquid/liquid.h>

// user-defined utility callback function
float myutility(void * _userdata, float * _v, unsigned int _n)
{
    float u = 0.0f;
    unsigned int i;
    for (i=0; i<_n; i++)
        u += fabsf(_v[i]);
    return u;
}

int main() {
    unsigned int num_parameters = 8;    // search dimensionality
    unsigned int num_iterations = 100;  // number of iterations to run
    float target_utility = 0.01f;       // target utility

    float v[num_parameters];            // optimum vector

    // ... intialize v ...

    // create gradsearch object
    gradsearch gs = gradsearch_create(NULL,
                                      v,
                                      num_parameters,
                                      &myutility,
                                      LIQUID_OPTIM_MINIMIZE,
                                      NULL);

    // execute batch search
    gradsearch_execute(gs, num_iterations, target_utility);

    // clean it up
    gradsearch_destroy(gs);
}
