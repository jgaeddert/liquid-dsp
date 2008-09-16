//
//
//

#include <stdio.h>

#include "sbuffer.h"

int main() {
    float v[] = {1, 2, 3, 4, 5, 6, 7, 8};
    float *r; // reader
    unsigned int n;

    sbuffer cb = sbuffer_create(10);

    sbuffer_write(cb, v, 4);
    sbuffer_write(cb, v, 4);

    sbuffer_read(cb, &r, &n);
    printf("sbuffer read %u elements\n", n);
    unsigned int i;
    for (i=0;i<n;i++)
        printf("  %u : %f\n", i, r[i]);

    sbuffer_release(cb);

    sbuffer_write(cb, v, 8);

    sbuffer_print(cb);

    sbuffer_destroy(cb);

    printf("done.\n");
    return 0;
}


