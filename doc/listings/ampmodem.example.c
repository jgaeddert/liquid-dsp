// file: doc/listings/ampmodem.example.c

int main() {
    float mod_index = 0.1f;         // modulation index (bandwidth)
    liquid_modem_amtype type = LIQUID_MODEM_AM_USB;
    int suppressed_carrier = 0;     // suppress the carrier?

    // create mod/demod objects
    ampmodem mod   = ampmodem_create(mod_index, type, suppressed_carrier);
    ampmodem demod = ampmodem_create(mod_index, type, suppressed_carrier);

    float x;                // input
    float complex y;        // modulated
    float z;                // output

    // repeat as necessary
    {
        // modulate signal
        ampmodem_modulate(mod, x, &y);

        // demodulate signal
        ampmodem_demodulate(demod, y, &z);
    }

    // clean up objects
    ampmodem_destroy(mod);
    ampmodem_destroy(demod);
}
