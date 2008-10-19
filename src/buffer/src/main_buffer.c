//
//
//

#include <stdio.h>

#include "buffer.h"

int main() {
    float v[] = {1, 2, 3, 4, 5, 6, 7, 8};
    float *r; // reader
    unsigned int n=3;

    fbuffer cb = fbuffer_create(CIRCULAR, 10);
    //fbuffer cb = fbuffer_create(STATIC, 10);

    fbuffer_zero(cb);
    fbuffer_push(cb, 1);
    fbuffer_push(cb, 2);
    fbuffer_push(cb, 3);
    fbuffer_debug_print(cb);
    return 0;
    unsigned int j;
    for (j=0; j<10; j++)
        fbuffer_push(cb, 100 + j);

    fbuffer_debug_print(cb);
    return 0;

    fbuffer_write(cb, v, 4);
    fbuffer_read(cb, &r, &n);
    printf("buffer read %u elements\n", n);
    unsigned int i;
    for (i=0;i<n;i++)
        printf("  %u : %f\n", i, r[i]);

    fbuffer_release(cb, 2);

    fbuffer_write(cb, v, 8);

    fbuffer_debug_print(cb);
    fbuffer_print(cb);

    fbuffer_destroy(cb);

    printf("done.\n");
    return 0;
}


