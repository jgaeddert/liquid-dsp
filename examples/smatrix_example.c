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

    smatrix q = smatrix_create(M,N);

    printf("\ncompact form:\n");
    smatrix_print(q);

    printf("\nexpanded form:\n");
    smatrix_print_expanded(q);

    smatrix_destroy(q);

    printf("done.\n");
    return 0;
}

