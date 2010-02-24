//
// firdespm.c
//
// Finite impulse response filter design using Parks-McClellan
// algorithm.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"

int main() {
    unsigned int n=24;
    float fp = 0.08f;
    float fs = 0.16f;
    float K  = 1.0f;

    float h[n];
    firdespm(n,fp,fs,K,h);

    printf("done.\n");
    return 0;
}

