// file: nco_test.c
//
// Tests simple nco functionality
//
// Complile and run:
//   $ gcc nco_test.c -lliquid -o ncotest
//   $ ./ncotest

#include <stdio.h>
#include <math.h>
#include "liquid.h"

int main() {
    struct nco_s nco_struct;
    nco p = &nco_struct;
    nco_init(p);
    nco_set_phase(p, 0.0f);
    nco_set_frequency(p, M_PI/10);

    unsigned int i;
    float s, c;
    for (i=0; i<11; i++) {
        nco_sincos(p, &s, &c);
        printf("  %3u: exp(j*%8.5f) = %8.5f + j %8.5f\n",
                i, p->theta, c, s);
        nco_step(p);
    }

    printf("done.\n");
    return 0;
}
