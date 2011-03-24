// file:    doc/listings/nco.c
// build:   gcc -c -o nco.c.o nco.c
// link:    gcc -lm -lc -lliquid nco.c.o -o nco

#include <stdio.h>
#include <math.h>
#include <liquid/liquid.h>
#include <complex.h>

int main() {
    // create nco object and initialize
    nco_crcf n = nco_crcf_create(LIQUID_NCO);
    nco_crcf_set_phase(n,0.3f);

    // Test native C complex data type
    float complex x;
    nco_crcf_cexpf(n, &x);
    printf("C native complex:   %12.8f + j%12.8f\n", crealf(x), cimagf(x));

    // destroy nco object
    nco_crcf_destroy(n);

    printf("done.\n");
    return 0;
}
