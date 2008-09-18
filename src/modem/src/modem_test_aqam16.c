// file: modem_test_aqam.c
//
// Tests simple modulation/demodulation without noise or phase
// offset, counting the number of resulting symbol errors.
//
// Complile and run:
//   $ gcc modem_test_aqam.c -lliquid -o modemtest
//   $ ./modemtest

#include "modem.h"

int main() {
    // effectively square 16-QAM
    q32_t levels_i[4] = {500, 500, 1500, 1500};
    q32_t levels_q[4] = {500, 1500, 500, 1500};
    unsigned int bits_per_symbol = 4;

    // create modulator
    modem_q32* mod = modem_create_arb_q32(bits_per_symbol);
    modem_arb_init_q32(mod, levels_i, levels_q, 4);

    // create demodulator
    modem_q32* demod = modem_create_arb_q32(bits_per_symbol);
    modem_arb_init_q32(demod, levels_i, levels_q, 4);

    unsigned int i; // modulated symbol
    unsigned int s; // demodulated symbol
    q32_t I, Q;
    unsigned int num_errors = 0;

    for (i=0; i<(mod->M); i++) {
        modulate_arb_q32(mod, i, &I, &Q);
        demodulate_arb_q32(demod, I, Q, &s);
        num_errors += ( i != s );
    }
    printf("num errors: %u\n", num_errors);

    free_modem_q32(mod);
    free_modem_q32(demod);
    return 0;
}
