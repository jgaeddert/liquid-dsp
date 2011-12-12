// file: doc/listings/modem.example.c
#include <liquid/liquid.h>

int main() {
    // create mod/demod objects
    unsigned int bps=2;
    modulation_scheme ms = LIQUID_MODEM_PSK;

    // create the modem objects
    modem mod   = modem_create(ms, bps);    // modulator
    modem demod = modem_create(ms, bps);    // demodulator
    modem_print(mod);

    unsigned int sym_in;    // input symbol
    float complex x;        // modulated sample
    unsigned int sym_out;   // demodulated symbol

    // ...repeat as necessary...
    {
        // modulate symbol
        modem_modulate(mod, sym_in, &x);

        // demodulate symbol
        modem_demodulate(demod, x, &sym_out);
    }

    // destroy modem objects
    modem_destroy(mod);
    modem_destroy(demod);
}
