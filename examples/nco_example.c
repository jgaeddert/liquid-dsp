// 
// nco_example.c
//
// This example demonstrates the most basic functionality of the
// numerically-controlled oscillator (NCO) object.
//

#include <stdio.h>
#include <math.h>
#include "liquid.h"

int main() {
    // create the NCO object
    nco p = nco_create(LIQUID_NCO);
    nco_set_phase(p, 0.0f);
    nco_set_frequency(p, M_PI/10);

    unsigned int i;
    float s, c;
    for (i=0; i<11; i++) {
        nco_sincos(p, &s, &c);
        printf("  %3u : %8.5f + j %8.5f\n",
                i, c, s);
        nco_step(p);
    }

    printf("done.\n");
    return 0;
}
