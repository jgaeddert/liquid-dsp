//
//
//

#include <stdio.h>

#include "../src/nco.h"

#include "../../random/src/random.h" // noise generator

int main() {
    // parameters
    float phase_offset = 0.1f;
    float frequency_offset = 0.0f;
    float SNRdB = 40.0f;
    float pll_bandwidth = 1e-4f;
    unsigned int n=200;     // number of iterations
    unsigned int d=10;      // print every "d" lines

    // objects
    nco nco_tx = nco_create();
    nco nco_rx = nco_create();
    pll pll_rx = pll_create();

    // initialize objects
    nco_set_phase(nco_tx, phase_offset);
    nco_set_frequency(nco_tx, frequency_offset);
    pll_set_bandwidth(pll_rx, pll_bandwidth);

    float noise_power = powf(10.0f, -SNRdB/20.0f);

    // run loop
    unsigned int i;
    float phase_error;
    float complex r, v;
    for (i=0; i<n; i++) {
        // received complex signal
        r = nco_cexpf(nco_tx);
        v = nco_cexpf(nco_rx);

        // add complex white noise
        //r += crandnf() * noise_power;

        // perfect error estimation
        phase_error = nco_tx->theta - nco_rx->theta;

        // imperfect error estimation
        //phase_error = cargf(r*v);

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

    nco_destroy(nco_tx);
    nco_destroy(nco_rx);
    pll_destroy(pll_rx);

    printf("done.\n");
    return 0;
}
