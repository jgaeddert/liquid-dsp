//
//
//

#include <stdio.h>

#include "cbuffer.h"

int main() {
    float v[] = {1, 2, 3, 4, 5, 6, 7, 8};
    float *r; // reader
    unsigned int n=3;

    cbuffer cb = cbuffer_create(10);

    cbuffer_write(cb, v, 4);
    cbuffer_read(cb, &r, &n);
    printf("cbuffer read %u elements\n", n);
    unsigned int i;
    for (i=0;i<n;i++)
        printf("  %u : %f\n", i, r[i]);

    cbuffer_release(cb, 2);

    cbuffer_write(cb, v, 8);

    cbuffer_debug_print(cb);
    cbuffer_print(cb);

    cbuffer_destroy(cb);

    printf("done.\n");
    return 0;
}


