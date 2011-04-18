// file: doc/listings/nco.example.c
#include <liquid/liquid.h>

int main() {
    // create the NCO object
    nco_crcf q = nco_crcf_create(LIQUID_NCO);
    nco_crcf_set_phase(q, 0.0f);
    nco_crcf_set_frequency(q, 0.13f);

    // output complex exponential
    float complex x;

    // repeat as necessary
    {
        // increment internal phase
        nco_crcf_step(q);

        // compute complex exponential
        nco_crcf_cexpf(q, &x);
    }

    // destroy nco object
    nco_crcf_destroy(q);
}
