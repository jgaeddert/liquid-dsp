//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "liquid.h"

int main() {
    unsigned int N=8;
    unsigned char x[N], y[N], z[N];

    unsigned int i;

    for (i=0; i<N; i++)
        x[i] = rand() % 0x0100;

    // scramble x
    memcpy(y,x,N);
    scramble_data(y,N);

    // unscramble y
    memcpy(z,y,N);
    unscramble_data(z,N);

    // print results
    printf("i\tx\ty\tz\n");
    printf("--\t--\t--\t--\n");
    for (i=0; i<N; i++)
        printf("%u\t%x\t%x\t%x\n", i, x[i], y[i], z[i]);

    printf("done.\n");
    return 0;
}

