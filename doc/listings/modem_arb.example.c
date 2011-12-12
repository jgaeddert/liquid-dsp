// file: doc/listings/modem_arb.example.c
#include <liquid/liquid.h>

int main() {
    // set modulation depth (bits/symbol)
    unsigned int bps=4;

    // create the arbitrary modem objects
    modem mod   = modem_create(LIQUID_MODEM_ARB, bps);  // modulator
    modem demod = modem_create(LIQUID_MODEM_ARB, bps);  // demodulator

    float complex constellation[1<<bps];
    // ... (initialize constellation) ...
    modem_arb_init(mod,   constellation, 1<<bps);
    modem_arb_init(demod, constellation, 1<<bps);

    // ... (modulate and demodulate as before) ...

    // destroy modem objects
    modem_destroy(mod);
    modem_destroy(demod);
}
