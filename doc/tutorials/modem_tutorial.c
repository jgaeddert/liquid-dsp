#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include <liquid/liquid.h>

int main() {
    // 
    unsigned int bps = 2;
    modulation_scheme ms = LIQUID_MODEM_PSK;
    float SNRdB = 3.0f;
    unsigned int num_symbols = 1000;

    // create the modem objects
    modem mod   = modem_create(ms, bps);
    modem demod = modem_create(ms, bps);

    modem_print(mod);

    float nstd = powf(10.0f, -SNRdB/20.0f);

    unsigned int i; // modulated symbol
    unsigned int s0; // demodulated symbol
    unsigned int s1; // demodulated symbol
    unsigned int M = 1<<bps;
    float complex x;
    unsigned int num_sym_errors = 0;
    unsigned int num_bit_errors = 0;

    for (i=0; i<num_symbols; i++) {
        
        s0 = rand()%M;
        modem_modulate(mod, s0, &x);

        // add noise
        x += nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;

        modem_demodulate(demod, x, &s1);

        num_sym_errors += s0 == s1 ? 0 : 1;
        num_bit_errors += count_bit_errors(s0,s1);
    }
    printf("num sym errors: %4u / %4u\n", num_sym_errors, num_symbols);
    printf("num bit errors: %4u / %4u\n", num_bit_errors, num_symbols*bps);


    modem_destroy(mod);
    modem_destroy(demod);

    printf("done.\n");
    return 0;
}
