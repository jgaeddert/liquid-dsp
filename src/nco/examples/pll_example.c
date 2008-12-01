//
// pll_example.c
// 
// Demonstrates a basic phase-locked loop to track the
// phase of a sinusoid.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../src/nco.h"

// DEBUG: print every output line in octave-friendly format
#define DEBUG

int main() {
    srand( time(NULL) );
    // parameters
    float phase_offset = M_PI/2;
    float frequency_offset = 0.0f;
    float pll_bandwidth = 1e-2f;
    unsigned int n=250;     // number of iterations
    unsigned int d=10;      // print every "d" lines
#ifdef DEBUG
    FILE * debug_file = fopen("debug_pll_phase.m","w");
    fprintf(debug_file, "clear all;\n");
    fprintf(debug_file, "phi=zeros(1,%u);\n",n);
#endif

    // objects
    nco nco_tx = nco_create();
    nco nco_rx = nco_create();
    pll pll_rx = pll_create();

    // initialize objects
    nco_set_phase(nco_tx, phase_offset);
    nco_set_frequency(nco_tx, frequency_offset);
    pll_set_bandwidth(pll_rx, pll_bandwidth);

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
        r = nco_cexpf(nco_tx);
        v = nco_cexpf(nco_rx);

        // error estimation
        phase_error = cargf(r*conjf(v));

        // perfect error estimation
        //phase_error = nco_tx->theta - nco_rx->theta;

#ifdef DEBUG
        // print every line in a format that octave can read
        fprintf(debug_file, "phi(%u) = %10.6E;\n", i+1, phase_error);
#endif
        if ((i+1)%d == 0 || i==n-1) {
            printf("  %4u: e_hat : %6.3f, freq error : %6.3f, phase error : %6.3f\n",
                    i+1,                                // iteration
                    phase_error,                        // estimated phase error
                    nco_tx->d_theta - nco_rx->d_theta,  // true phase error
                    nco_tx->theta - nco_rx->theta);     // true frequency error
        }

        // update NCO objects
        nco_step(nco_tx);
        pll_step(pll_rx, nco_rx, phase_error);
        nco_step(nco_rx);
    }
#ifdef DEBUG
    fprintf(debug_file, "plot(phi);\n");
    fclose(debug_file);
#endif

    nco_destroy(nco_tx);
    nco_destroy(nco_rx);
    pll_destroy(pll_rx);

    printf("done.\n");
    return 0;
}
