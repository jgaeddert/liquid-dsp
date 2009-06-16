//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "liquid.internal.h"

int main() {
    srand(time(NULL));
    unsigned int n=16;
    unsigned int num_bits = 64;
    unsigned int max_bits = 8;  // maximum bits per channel
    float e[n];
    unsigned int i;
    for (i=0; i<n; i++) {
        e[i] = expf(2*randnf());
    }

    // bit allocation
    unsigned int k[n];

    fbasc_compute_bit_allocation(n,e,num_bits,max_bits,k);

    for (i=0; i<n; i++)
        printf("e[%3u] = %12.8f : %3u\n", i, e[i], k[i]);

    printf("done.\n");
    return 0;
}

