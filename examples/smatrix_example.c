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

    smatrix_print(q);

    smatrix_destroy(q);

    printf("done.\n");
    return 0;
}

