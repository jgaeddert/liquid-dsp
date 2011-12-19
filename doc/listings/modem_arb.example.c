// file: doc/listings/modem_arb.example.c
#include <liquid/liquid.h>

int main() {
    // set modulation depth (bits/symbol)
    unsigned int bps=4;
    float complex constellation[1<<bps];

    // ... (initialize constellation) ...

    // create the arbitrary modem objects
    modem mod   = modem_create_arbitrary(constellation, 1<<bps);
    modem demod = modem_create_arbitrary(constellation, 1<<bps);

    // ... (modulate and demodulate as before) ...

    // destroy modem objects
    modem_destroy(mod);
    modem_destroy(demod);
}
