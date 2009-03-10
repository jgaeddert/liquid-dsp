//
// Recursive least-squares equalizer
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct EQRLS(_s) {
    unsigned int p;     // filter order
    float lambda;       // RLS forgetting factor
    float delta;        // RLS initialization factor

    T * w0, * w1;       // weights [px1]
    T * P0, * P1;       // recursion matrix [pxp]
    T * g;              // gain vector [px1]
};

EQRLS() EQRLS(_create)(unsigned int _p)
{
    EQRLS() eq = (EQRLS()) malloc(sizeof(struct EQRLS(_s)));
    return eq;
}

void EQRLS(_destroy)(EQRLS() _eq)
{
    free(_eq->w0);
    free(_eq->w1);
    free(_eq->P0);
    free(_eq->P1);
    free(_eq->g);
    free(_eq);
}

void EQRLS(_print)(EQRLS() _eq)
{
    printf("equalizer (RLS):\n");
    printf("    order:      %u\n", _eq->p);
}

void EQRLS(_execute)(EQRLS() _eq)
{
}

