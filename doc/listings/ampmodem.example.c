// file: doc/listings/ampmodem.example.c
#include <liquid/liquid.h>

int main() {
    float mod_index = 0.1f;         // modulation index (bandwidth)
    float fc = 0.0f;                // AM carrier
    liquid_ampmodem_type type = LIQUID_AMPMODEM_USB;
    int suppressed_carrier = 0;     // suppress the carrier?

    // create mod/demod objects
    ampmodem mod   = ampmodem_create(mod_index, fc, type, suppressed_carrier);
    ampmodem demod = ampmodem_create(mod_index, fc, type, suppressed_carrier);

    float s;                // input message
    float complex x;        // modulated
    float y;                // output/demodulated message

    // repeat as necessary
    {
        // modulate signal
        ampmodem_modulate(mod, s, &x);

        // demodulate signal
        ampmodem_demodulate(demod, x, &y);
    }

    // clean up objects
    ampmodem_destroy(mod);
    ampmodem_destroy(demod);
}
