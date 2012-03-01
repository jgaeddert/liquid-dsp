//
// modular_arithmetic_example.c
//
// This example demonstates some modular arithmetic functions.
//

#include <stdio.h>
#include "liquid.h"

int main() {
    unsigned int n=280;

    // compute factors of n
    unsigned int factors[LIQUID_MAX_FACTORS];
    unsigned int num_factors=0;
    liquid_factor(n,factors,&num_factors);

    printf("factors of %u:\n", n);
    unsigned int i;
    for (i=0; i<num_factors; i++)
        printf("%3u\n", factors[i]);

    printf("done.\n");
    return 0;
}

