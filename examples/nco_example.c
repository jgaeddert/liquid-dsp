// 
// nco_example.c
//
// This example demonstrates the most basic functionality of the
// numerically-controlled oscillator (NCO) object.
//
// SEE ALSO: nco_pll_example.c
//           nco_pll_modem_example.c
//

#include <stdio.h>
#include <math.h>
#include "liquid.h"

int main() {
    // create the NCO object
    nco_crcf p = nco_crcf_create(LIQUID_NCO);
    nco_crcf_set_phase(p, 0.0f);
    nco_crcf_set_frequency(p, M_PI/10);

    unsigned int i;
    float s, c;
    for (i=0; i<11; i++) {
        nco_crcf_sincos(p, &s, &c);
        printf("  %3u : %8.5f + j %8.5f\n",
                i, c, s);
        nco_crcf_step(p);
    }

    // clean up allocated memory
    nco_crcf_destroy(p);

    printf("done.\n");
    return 0;
}
