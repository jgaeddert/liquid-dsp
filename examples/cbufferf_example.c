//
// cbufferf_example.c
//
// This example demonstrates the circular buffer object on
// floating-point data.
//
// SEE ALSO: wdelayf_example.c
//           windowf_example.c

#include <stdio.h>

#include "liquid.h"

int main() {
    float v[] = {1, 2, 3, 4, 5, 6, 7, 8};
    float *r; // reader
    unsigned int n=3;

    cbufferf cb = cbufferf_create(10);

    cbufferf_write(cb, v, 4);
    cbufferf_read(cb, &r, &n);
    printf("cbufferf read %u elements\n", n);
    unsigned int i;
    for (i=0;i<n;i++)
        printf("  %u : %f\n", i, r[i]);

    cbufferf_release(cb, 2);

    cbufferf_write(cb, v, 8);

    cbufferf_debug_print(cb);
    cbufferf_print(cb);

    cbufferf_destroy(cb);

    printf("done.\n");
    return 0;
}


