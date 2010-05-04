//
// decim_rrrf_example.c
//

#include <stdio.h>

#include "liquid.h"

int main() {
    float h[] = {0.5, 0.5, 0.5, 0.5};
    unsigned int h_len = 4; // filter length
    unsigned int D = 4;     // decim_rrrf factor
    decim_rrrf q = decim_rrrf_create(D,h,h_len);

    float x[16] = { 
        0.5,  0.5,  0.5,  0.5,
       -0.5, -0.5, -0.5, -0.5,
        0.5,  0.5,  0.5,  0.5,
        0.5,  0.5,  0.5,  0.5};

    float y[4];

    unsigned int i, n=0;
    for (i=0; i<4; i++) {
        decim_rrrf_execute(q, &x[n], &y[i], D-1);
        n+=D;
    }

    printf("h(t) :\n");
    for (i=0; i<4; i++)
        printf("  h(%2u) = %8.4f;\n", i+1, h[i]);

    printf("x(t) :\n");
    for (i=0; i<16; i++)
        printf("  x(%2u) = %8.4f;\n", i+1, x[i]);

    printf("y(t) :\n");
    for (i=0; i<4; i++)
        printf("  y(%2u) = %8.4f;\n", i+1, y[i]);

    decim_rrrf_destroy(q);
    printf("done.\n");
    return 0;
}
