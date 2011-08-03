// file: doc/listings/gmskmodem.example.c
#include <liquid/liquid.h>

int main() {
    // options
    unsigned int k=4;       // filter samples/symbol
    unsigned int m=3;       // filter delay (symbols)
    float BT=0.3f;          // bandwidth-time product

    // create modulator/demodulator objects
    gmskmod mod   = gmskmod_create(k, m, BT);
    gmskdem demod = gmskdem_create(k, m, BT);

    unsigned int i;
    unsigned int sym_in;    // input data symbol
    float complex x[k];     // modulated samples
    unsigned int sym_out;   // demodulated data symbol

    {
        // generate random symbol {0,1}
        sym_in = rand() % 2;

        // modulate
        gmskmod_modulate(mod, sym_in, x);

        // demodulate
        gmskdem_demodulate(demod, x, &sym_out);
    }

    // destroy modem objects
    gmskmod_destroy(mod);
    gmskdem_destroy(demod);
}
