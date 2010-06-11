//
//
//

#include <stdio.h>
#include <complex.h>
#define J _Complex_I

#include "liquid.h"


int main() {
    complex float v[8];
    v[0] = 1 + J*1;
    v[1] = 2 + J*2;
    v[2] = 3 + J*3;
    v[3] = 4 + J*4;
    v[4] = 5 + J*5;
    v[5] = 6 + J*6;
    v[6] = 7 + J*7;
    v[7] = 8 + J*8;
    complex float *r; // reader
    unsigned int n=3;

    //buffer cb = buffercf_create(CIRCULAR, 10);
    buffercf cb = buffercf_create(STATIC, 10);

    buffercf_write(cb, v, 4);
    buffercf_read(cb, &r, &n);
    printf("buffer read %u elements\n", n);
    unsigned int i;
    for (i=0;i<n;i++)
        printf("  %u : %f + j%f\n", i, crealf(r[i]), cimagf(r[i]));

    buffercf_release(cb, 2);

    buffercf_write(cb, v, 8);

    buffercf_debug_print(cb);
    buffercf_print(cb);

    buffercf_destroy(cb);

    printf("done.\n");
    return 0;
}


