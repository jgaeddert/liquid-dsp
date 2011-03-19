// file: doc/tutorials/pll_tutorial.c
#include <stdio.h>
#include <complex.h>
#include <math.h>
#include <liquid/liquid.h>

int main() {
    // simulation parameters
    float phase_offset      = 0.8f;     // initial phase offset
    float frequency_offset  = 0.01f;    // initial frequency offset
    float wn                = 0.10f;    // pll bandwidth
    float zeta              = 0.707f;   // pll damping factor
    float K                 = 1000;     // pll loop gain
    unsigned int n          = 40;       // number of iterations

    // generate IIR loop filter coefficients
    float b[3];     // feedforward coefficients
    float a[3];     // feedback coefficients
    iirdes_pll_active_lag(wn, zeta, K, b, a);

    // create and print the loop filter object
    iirfilt_rrrf loopfilter = iirfilt_rrrf_create(b,3,a,3);
    iirfilt_rrrf_print(loopfilter);

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

        // run error through loop filter
        iirfilt_rrrf_execute(loopfilter, phase_error, &phi_hat);

        // print results to screen
        printf("%3u : phase = %12.8f, error = %12.8f\n", i, phi_hat, phase_error);
    }

    // destroy IIR filter object
    iirfilt_rrrf_destroy(loopfilter);

    printf("done.\n");
    return 0;
}
