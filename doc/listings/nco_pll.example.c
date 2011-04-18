// file: doc/listings/nco_pll.example.c
#include <liquid/liquid.h>

int main() {
    // create nco objects
    nco_crcf nco_tx = nco_crcf_create(LIQUID_VCO);    // transmit NCO
    nco_crcf nco_rx = nco_crcf_create(LIQUID_VCO);    // receive NCO

    // ... initialize objects ...

    float complex * x;
    unsigned int i;
    // loop as necessary
    {
        // tx : generate complex sinusoid
        nco_crcf_cexpf(nco_tx, &x[i]);

        // compute phase error
        float dphi = nco_crcf_get_phase(nco_tx) -
                     nco_crcf_get_phase(nco_rx);

        // update pll
        nco_crcf_pll_step(nco_rx, dphi);

        // update nco objects
        nco_crcf_step(nco_tx);
        nco_crcf_step(nco_rx);
    }

    // destry nco object
    nco_crcf_destroy(nco_tx);
    nco_crcf_destroy(nco_rx);
}
