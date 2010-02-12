// 
// poly_findroots_example.c
//
// test polynomial root-finding algorithm(s)
//

#include <stdio.h>

#include "liquid.h"

int main() {
    float complex p[4] = {-5, 3, -3, 1};
    float complex roots[3];

    cfpoly_findroots(p,4,roots);

    unsigned int i;
    for (i=0; i<3; i++)
        printf("r[%3u] = %12.8f + j*%12.8f\n", i, crealf(roots[i]), cimagf(roots[i]));

    return 0;
}

