//
//
//

#include <stdio.h>

#include "buffer.h"

int main() {
    float v[] = {1, 2, 3, 4, 5, 6, 7, 8};
    float *r; // reader
    unsigned int n=3;

    //buffer cb = buffer_create(CIRCULAR, 10);
    buffer cb = buffer_create(STATIC, 10);

    buffer_write(cb, v, 4);
    buffer_read(cb, &r, &n);
    printf("buffer read %u elements\n", n);
    unsigned int i;
    for (i=0;i<n;i++)
        printf("  %u : %f\n", i, r[i]);

    buffer_release(cb, 2);

    buffer_write(cb, v, 8);

    buffer_debug_print(cb);
    buffer_print(cb);

    buffer_destroy(cb);

    printf("done.\n");
    return 0;
}


