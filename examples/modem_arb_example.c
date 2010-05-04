//
// modem_arb_example.c
//
// This example demonstrates the functionality of the arbitrary
// modem, a digital modulator/demodulator object with signal
// constellation points chosen arbitrarily.  A simple bit-error
// rate simulation is then run to test the performance of the
// modem.  The results are written to a file.
// SEE ALSO: modem_example.c
//

#include <stdio.h>
#include "liquid.h"

#define OUTPUT_FILENAME "modem_arb_example.m"

int main() {
    // options
    unsigned int bps=6;         // bits per symbol
    float complex * constellation = (float complex*)modem_arb_vt64;
    unsigned int n=1024;        // number of data points to evaluate

    // derived values
    unsigned int M = 1<<bps;    // constellation size

    // open output file
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"bps = %u;\n", bps);
    fprintf(fid,"M = %u;\n", M);

    // create mod/demod objects
    modem mod   = modem_create(MOD_ARB, bps);
    modem demod = modem_create(MOD_ARB, bps);

    // initialize mod/demod objects (NOTE: arbitrary modem
    // objects MUST be initialized before use)
    modem_arb_init(mod,constellation,M);
    modem_arb_init(demod,constellation,M);

    modem_print(mod);

    unsigned int i;
    float complex x;
    unsigned int num_errors = 0;

    // run simple BER simulation
    num_errors = 0;
    unsigned int sym_in;
    unsigned int sym_out;
    for (i=0; i<n; i++) {
        // generate and modulate random symbol
        sym_in = modem_gen_rand_sym(mod);
        modem_modulate(mod, sym_in, &x);

        // add noise
        cawgn(&x,0.05);

        // demodulate
        modem_demodulate(demod, x, &sym_out);

        // accumulate errors
        num_errors += count_bit_errors(sym_in,sym_out);
        fprintf(fid,"x(%3u) = %12.4e + j*%12.4e;\n", i+1,
                                                     crealf(x),
                                                     cimagf(x));
    }
    printf("num bit errors: %4u / %4u\n", num_errors, bps*n);

    // plot results
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(x,'x','MarkerSize',1);\n");
    fprintf(fid,"xlabel('in-phase');\n");
    fprintf(fid,"ylabel('quadrature phase');\n");
    fprintf(fid,"title(['Arbitrary ' num2str(M) '-QAM']);\n");
    fprintf(fid,"axis([-1 1 -1 1]*1.9);\n");
    fprintf(fid,"axis square;\n");
    fprintf(fid,"grid on;\n");
    fclose(fid);

    modem_destroy(mod);
    modem_destroy(demod);
    return 0;
}
