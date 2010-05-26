#include <liquid/liquid.h>
// ...
{
    // create nco objects
    nco nco_tx = nco_create(LIQUID_VCO);    // transmit NCO
    nco nco_rx = nco_create(LIQUID_VCO);    // receive NCO

    // ... initialize objects ...

    // loop as necessary
    {
        // tx : generate complex sinusoid
        nco_cexpf(nco_tx, &x[i]);

        // compute phase error
        float dphi = nco_get_phase(nco_tx) -
                     nco_get_phase(nco_rx);

        // update pll
        nco_pll_step(nco_rx, dphi);

        // update nco objects
        nco_step(nco_tx);
        nco_step(nco_rx);
    }

    // destry nco object
    nco_destroy(nco_tx);
    nco_destroy(nco_rx);
}
