//
// AGC test
//

#include <stdio.h>
#include "agc.h"

int main() {
    agc p = agc_create(100.0f, 0.01f);

    unsigned int i;
    float complex x = 1.2f + _Complex_I*3.9f, y=x;
    printf("i    \tlevel\n");
    printf("-----\t-----\n");
    for (i=0; i<100; i++) {
        if (((i+1)%5)==0)
            printf("%u\t%f\n", i+1, cabsf(y));
        agc_apply_gain(p, x, &y);
    }

    agc_free(p);

    printf("done.\n");
    return 0;
}

