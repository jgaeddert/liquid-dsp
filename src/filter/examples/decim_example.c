//
//
//

#include <stdio.h>

#include "../src/filter.h"

int main() {
    float h[] = {0.5, 0.5, 0.5, 0.5};
    unsigned int h_len = 4; // filter length
    unsigned int D = 4;     // decim factor
    decim q = decim_create(D,h,h_len);

    float x[16] = { 
        0.5,  0.5,  0.5,  0.5,
       -0.5, -0.5, -0.5, -0.5,
        0.5,  0.5,  0.5,  0.5,
        0.5,  0.5,  0.5,  0.5};

    float y[4];

    unsigned int i, n=0;
    for (i=0; i<4; i++) {
        decim_execute(q, &x[n], &y[i], D-1);
        n+=D;
    }

    for (i=0; i<4; i++)
        printf("  y(%u) = %8.4f;\n", i+1, y[i]);

    decim_print(q);

    decim_destroy(q);
    printf("done.\n");
    return 0;
}
