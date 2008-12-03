//
//
//

#include <stdio.h>

#include "../src/filter.h"

int main() {
    float h[] = {0, 0.25, 0.5, 0.75, 1.0, 0.75, 0.5, 0.25, 0};
    unsigned int h_len = 9; // filter length
    unsigned int M = 4;     // interp factor
    interp q = interp_create(4,h,h_len);

    float x[] = {1.0, -1.0, 1.0, 1.0};
    float y[16];

    unsigned int i, n=0;
    for (i=0; i<4; i++) {
        interp_execute(q, x[i], &y[n]);
        n+=M;
    }

    for (i=0; i<16; i++)
        printf("  y(%u) = %8.4f;\n", i+1, y[i]);

    interp_print(q);

    interp_destroy(q);
    printf("done.\n");
    return 0;
}
