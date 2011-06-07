// 
// cgsolve_example.c
//

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "liquid.h"

int main() {
    float A[4] = {
        3, 2,
        2, 6};

    float b[2] = {
        2,
        -8};

    float x[2];

    // solve symmetric positive-definite system of equations
    matrixf_cgsolve(A, 2, b, x, NULL);

    matrixf_print(x, 2, 1);

    printf("done.\n");
    return 0;
}

