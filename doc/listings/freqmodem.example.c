// file: doc/listings/freqmodem.example.c
#include <liquid/liquid.h>

int main() {
    float mod_index = 0.1f; // modulation index (bandwidth)
    float fc = 0.0f;        // FM carrier
    liquid_fmtype type = LIQUID_MODEM_FM_DELAY_CONJ;

    // create mod/demod objects
    freqmodem mod   = freqmodem_create(mod_index,fc,type);
    freqmodem demod = freqmodem_create(mod_index,fc,type);

    float s;                // input message
    float complex x;        // modulated
    float y;                // output/demodulated message

    // repeat as necessary
    {
        // modulate signal
        freqmodem_modulate(mod, s, &x);

        // demodulate signal
        freqmodem_demodulate(demod, x, &y);
    }

    // clean up objects
    freqmodem_destroy(mod);
    freqmodem_destroy(demod);
}
