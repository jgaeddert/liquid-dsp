//
// pll_example.c
// 
// Demonstrates a basic phase-locked loop to track the
// phase of a sinusoid.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "liquid.h"

// output to octave-friendly format
#define OUTPUT_FILENAME "pll_example.m"

int main() {
    srand( time(NULL) );
    // parameters
    float phase_offset = M_PI/2;
    float frequency_offset = 0.0f;
    float pll_bandwidth = 1e-3f;
    float pll_damping_factor = 4.0f;
    unsigned int n=250;     // number of iterations
    unsigned int d=10;      // print every "d" lines

    FILE * debug_file = fopen(OUTPUT_FILENAME,"w");
    fprintf(debug_file, "clear all;\n");
    fprintf(debug_file, "phi=zeros(1,%u);\n",n);

    // objects
    nco nco_tx = nco_create(LIQUID_VCO);
    nco nco_rx = nco_create(LIQUID_VCO);
    pll pll_rx = pll_create();

    // initialize objects
    nco_set_phase(nco_tx, phase_offset);
    nco_set_frequency(nco_tx, frequency_offset);
    pll_set_bandwidth(pll_rx, pll_bandwidth);
    pll_set_damping_factor(pll_rx, pll_damping_factor);

    // print parameters
    printf("PLL example :\n");
    printf("frequency offset: %6.3f, phase offset: %6.3f, pll b/w: %6.3f\n",
            frequency_offset, phase_offset, pll_bandwidth);

    // run loop
    unsigned int i;
    float phase_error;
    float complex r, v;
    for (i=0; i<n; i++) {
        // received complex signal
        nco_cexpf(nco_tx,&r);
        nco_cexpf(nco_rx,&v);

        // error estimation
        phase_error = cargf(r*conjf(v));

        // perfect error estimation
        //phase_error = nco_tx->theta - nco_rx->theta;

        // print every line in a format that octave can read
        fprintf(debug_file, "phi(%u) = %10.6E;\n", i+1, phase_error);
        if ((i+1)%d == 0 || i==n-1) {
            printf("  %4u: e_hat : %6.3f, freq error : %6.3f, phase error : %6.3f\n",
                    i+1,                                // iteration
                    phase_error,                        // estimated phase error
                    nco_get_frequency(nco_tx) - nco_get_frequency(nco_rx),  // true frequency error
                    nco_get_phase(nco_tx) - nco_get_phase(nco_rx));     // true phase error
        }

        // update NCO objects
        nco_step(nco_tx);
        pll_step(pll_rx, nco_rx, phase_error);
        nco_step(nco_rx);
    }
    fprintf(debug_file, "plot(phi);\n");
    fprintf(debug_file, "xlabel('time [sample]');\n");
    fprintf(debug_file, "ylabel('phase error [radians]')\n");
    fprintf(debug_file, "grid on;\n");
    fclose(debug_file);

    printf("output written to %s.\n", OUTPUT_FILENAME);

    nco_destroy(nco_tx);
    nco_destroy(nco_rx);
    pll_destroy(pll_rx);

    printf("done.\n");
    return 0;
}
