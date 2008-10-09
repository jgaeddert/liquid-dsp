//
// AGC test
//

#include <stdio.h>
#include "agc.h"

int main() {
    agc p = agc_create(1.0f, 0.01f);

    unsigned int i;
    float complex x = 0.1f, y;
    printf("i    \tlevel\n");
    printf("-----\t-----\n");
    for (i=0; i<20; i++) {
        printf("%u\t%f\n", i, crealf(x));
        agc_apply_gain(p, x, &y);
        x = y;
    }

    agc_free(p);

    printf("done.\n");
    return 0;
}

