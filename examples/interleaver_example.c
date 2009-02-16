//
//
//

#include <stdio.h>
#include <stdlib.h> // for rand()

#include "liquid.h"

int main() {
    unsigned int n=9;
    interleaver q = interleaver_create(n, INT_SEQUENCE);
    interleaver_print(q);
    interleaver_debug_print(q);

    unsigned char x[n];
    unsigned char y[n];
    unsigned char z[n];
    unsigned int i;
    for (i=0; i<n; i++)
        x[i] = rand()%256;

    interleaver_interleave(q,x,y);
    interleaver_deinterleave(q,y,z);
    //interleaver_print(q);

    unsigned int num_errors=0;
    printf("x\ty\tz\n");
    for (i=0; i<n; i++) {
        printf("%u\t%u\t%u\n", (unsigned int) (x[i]), (unsigned int) (y[i]), (unsigned int) (z[i]));
        //printf("y[%u] = %u\n", i, (unsigned int) (y[i]));
        //printf("y[%u] = %#0x\n", i, (unsigned int) (y[i]));
        num_errors += (x[i]==z[i]) ? 0 : 1;
    }
    printf("errors: %u / %u\n", num_errors, n);

    interleaver_destroy(q);

    printf("done.\n");
    return 0;
}

