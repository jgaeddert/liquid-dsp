//
// smatrix_example.c
//

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <math.h>

#include "liquid.internal.h"

int main(int argc, char*argv[]) {
    unsigned int M = 12;
    unsigned int N = 16;

    // create empty list
    smatrix q = smatrix_create(M,N);

    // set value(s)
    smatrix_set(q,1,2);
    smatrix_set(q,2,2);
    smatrix_set(q,2,3);
    smatrix_set(q,2,7);
    smatrix_set(q,2,8);
    smatrix_set(q,2,11);
    smatrix_clear(q,2,2);

    printf("\ncompact form:\n");
    smatrix_print(q);

    printf("\nexpanded form:\n");
    smatrix_print_expanded(q);

    printf("\ncertain values:\n");
    printf("  A[%2u,%2u] = %1u\n", 1, 1, smatrix_get(q,1,1));
    printf("  A[%2u,%2u] = %1u\n", 1, 2, smatrix_get(q,1,2));

    smatrix_destroy(q);

    printf("done.\n");
    return 0;
}

