// file: modem_test.c
//
// Tests simple modulation/demodulation without noise or phase
// offset, counting the number of resulting symbol errors.
//
// Complile and run:
//   $ gcc modem_test.c -lliquid -o modemtest
//   $ ./modemtest

#include <stdio.h>
#include "liquid.h"

int main() {
    // create mod/demod objects
    unsigned int bps=4;
    modem mod = modem_create(MOD_QAM, bps);
    modem demod = modem_create(MOD_QAM, bps);

    unsigned int i; // modulated symbol
    unsigned int s; // demodulated symbol
    unsigned int num_symbols = 1<<bps;
    float complex x;
    unsigned int num_errors = 0;

    for (i=0; i<num_symbols; i++) {
        modulate(mod, i, &x);
        demodulate(demod, x, &s);
        num_errors += count_bit_errors(i,s);
    }
    printf("num errors: %u\n", num_errors);

    modem_destroy(mod);
    modem_destroy(demod);
    return 0;
}
