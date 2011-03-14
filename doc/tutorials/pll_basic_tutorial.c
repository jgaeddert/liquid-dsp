// file: doc/tutorials/pll_basic_tutorial.c
#include <stdio.h>
#include <complex.h>
#include <math.h>
#include <liquid/liquid.h>

int main() {
    // simulation parameters
    float phase_offset      = 0.8f;     // initial phase offset
    float frequency_offset  = 0.01f;    // initial frequency offset
    unsigned int n          = 40;       // number of iterations

    float complex x   = 0;  // input sample
    float phase_error = 0;  // phase error estimate
    float phi_hat     = 0;  // output sample phase
    float complex y   = 0;  // output sample

    unsigned int i;
    for (i=0; i<n; i++) {
        // generate input sample
        x = cexpf(_Complex_I*(phase_offset + i*frequency_offset));

        // generate output sample
        y = cexpf(_Complex_I*phi_hat);

        // compute phase error
        phase_error = cargf(x*conjf(y));

        // print results to screen
        printf("%3u : phase = %12.8f, error = %12.8f\n", i, phi_hat, phase_error);
    }

    printf("done.\n");
    return 0;
}
