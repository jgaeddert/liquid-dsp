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
    unsigned int n=25;
    float fp = 0.1f;
    float fs = 0.2f;

    unsigned int num_bands = 2;
    float bands[4]   = {0.0f, fp, fs, 0.5f};
    float des[2]     = {1.0f, 0.0f};
    float weights[2] = {1.0f, 1.0f};
    liquid_firdespm_btype btype = LIQUID_FIRDESPM_BANDPASS;

    float h[n];
    firdespm q = firdespm_create(n,bands,des,weights,num_bands,btype);
    firdespm_print(q);
    firdespm_execute(q,h);
    firdespm_destroy(q);

    unsigned int i;
    for (i=0; i<n; i++)
        printf("%20.12f\n", h[i]);

    printf("done.\n");
    return 0;
}

