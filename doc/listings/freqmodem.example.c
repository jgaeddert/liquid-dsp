// file: doc/listings/freqmodem.example.c

int main() {
    float mod_index = 0.1f; // modulation index (bandwidth)
    float fc = 0.0f;        // FM carrier
    liquid_fmtype type = LIQUID_MODEM_FM_DELAY_CONJ;

    // create mod/demod objects
    freqmodem mod   = freqmodem_create(mod_index,fc,type);
    freqmodem demod = freqmodem_create(mod_index,fc,type);

    float x;                // input
    float complex y;        // modulated
    float z;                // output

    // repeat as necessary
    {
        // modulate signal
        freqmodem_modulate(mod, x, &y);

        // demodulate signal
        freqmodem_demodulate(demod, y, &z);
    }

    // clean up objects
    freqmodem_destroy(mod);
    freqmodem_destroy(demod);
}
