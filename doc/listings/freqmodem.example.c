// file: doc/listings/freqmodem.example.c
#include <liquid/liquid.h>

int main() {
    float kf = 0.02f;       // modulation index
    liquid_freqdem_type type = LIQUID_FREQDEM_DELAYCONJ;

    // create modulator/demodulator objects
    freqmod fmod = freqmod_create(kf);
    freqdem fdem = freqdem_create(kf, type);

    float m;                // input message
    float complex s;        // modulated signal
    float y;                // output/demodulated message

    // repeat as necessary
    {
        // modulate signal
        freqmod_modulate(fmod, m, &s);

        // demodulate signal
        freqdem_demodulate(fdem, s, &y);
    }

    // clean up objects
    freqmod_destroy(fmod);
    freqdem_destroy(fdem);
}
