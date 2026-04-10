const char __docstr__[] = "Demonstrate pi/4 differential QPSK modem.";

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "modem_pi4dqpsk_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(float,    SNRdB,        25, 's', "signal-to-noise ratio", NULL);
    liquid_argparse_add(unsigned, num_symbols, 800, 'n', "number of symbols", NULL);
    liquid_argparse_parse(argc,argv);

    // create the modem objects
    modemcf mod   = modemcf_create(LIQUID_MODEM_PI4DQPSK);
    modemcf demod = modemcf_create(LIQUID_MODEM_PI4DQPSK);
    modemcf_print(mod);

    // open output file
    FILE*fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all; close all;\n");
    fprintf(fid,"n=%u; sym_rx=zeros(1,n); sym_rec=zeros(1,n);\n", num_symbols);

    unsigned int i, index, s;
    liquid_float_complex sym_tx, sym_rx;
    unsigned int num_sym_errors = 0, num_bit_errors = 0;
    float nstd = powf(10.0f, -SNRdB/20.0f);
    float r_prime = 0.0f;
    liquid_float_complex s_prime;
    for (i=0; i<num_symbols; i++) {
        // modulate symbol
        index = rand() & 3;
        modemcf_modulate(mod, index, &sym_tx);

        // add noise
        sym_rx = sym_tx + nstd*(randnf() + _Complex_I*randnf()) * M_SQRT1_2;

        // demodulate using object
        modemcf_demodulate(demod, sym_rx, &s);

        // custom demod to show QPSK constellation
        s_prime = sym_rx * cexpf(-_Complex_I*r_prime);
        r_prime = cargf(sym_rx);

        // accumulate errors
        num_sym_errors += index != s;
        num_bit_errors += count_bit_errors(index,s);

        // write symbol to output file
        fprintf(fid,"sym_rx (%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(sym_rx), cimagf(sym_rx));
        fprintf(fid,"sym_rec(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(s_prime), cimagf(s_prime));
    }
    printf("num sym errors: %4u / %4u\n", num_sym_errors, num_symbols);
    printf("num bit errors: %4u / %4u\n", num_bit_errors, num_symbols*2);
    modemcf_destroy(mod);
    modemcf_destroy(demod);

    // plot results
    fprintf(fid,"\n\n");
    fprintf(fid,"figure('position',[100 100 800 400])\n");
    fprintf(fid,"subplot(1,2,1),");
    fprintf(fid,"  plot(real(sym_rx),imag(sym_rx),'o','MarkerSize',2);\n");
    fprintf(fid,"  axis([-1 1 -1 1]*1.5);\n");
    fprintf(fid,"  axis square;\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('in phase');\n");
    fprintf(fid,"  ylabel('quadrature phase');\n");
    fprintf(fid,"  title('Received');\n");
    fprintf(fid,"subplot(1,2,2),");
    fprintf(fid,"  plot(real(sym_rec),imag(sym_rec),'o','MarkerSize',2);\n");
    fprintf(fid,"  axis([-1 1 -1 1]*1.5);\n");
    fprintf(fid,"  axis square;\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('in phase');\n");
    fprintf(fid,"  ylabel('quadrature phase');\n");
    fprintf(fid,"  title('De-rotated');\n");
    fclose(fid);
    printf("results written to %s.\n", filename);
    return 0;
}
