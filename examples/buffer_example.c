//
//
//

#include <stdio.h>

#include "liquid.h"

int main() {
    float v[] = {1, 2, 3, 4, 5, 6, 7, 8};
    float *r; // reader
    unsigned int n=3;

    bufferf cb = bufferf_create(CIRCULAR, 10);
    //bufferf cb = bufferf_create(STATIC, 10);

    bufferf_zero(cb);
    bufferf_push(cb, 1);
    bufferf_push(cb, 2);
    bufferf_push(cb, 3);
    unsigned int j;
    for (j=0; j<10; j++)
        bufferf_push(cb, 100 + j);

    bufferf_write(cb, v, 4);
    bufferf_read(cb, &r, &n);
    printf("buffer read %u elements\n", n);
    unsigned int i;
    for (i=0;i<n;i++)
        printf("  %u : %f\n", i, r[i]);

    bufferf_release(cb, 2);

    bufferf_write(cb, v, 8);

    bufferf_debug_print(cb);
    bufferf_print(cb);

    bufferf_destroy(cb);

    printf("done.\n");
    return 0;
}


