// 
// modemq16_example.c
//
// Fixed-point linear modem example.
//

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include "liquid.h"

#define OUTPUT_FILENAME "modemq16_example.m"

// print usage/help message
void usage()
{
    printf("modemq16_example [options]\n");
    printf("  h     : print help\n");
    printf("  v/q   : verbose/quiet\n");
    printf("  m     : modulation scheme (qpsk default)\n");
    liquid_print_modulation_schemes();
}


int main(int argc, char*argv[])
{
    // create mod/demod objects
    modulation_scheme ms = LIQUID_MODEM_QPSK;
    int verbose = 1;

    int dopt;
    while ((dopt = getopt(argc,argv,"hvqm:")) != EOF) {
        switch (dopt) {
        case 'h':   usage();        return 0;
        case 'v':   verbose = 1;    break;
        case 'q':   verbose = 0;    break;
        case 'm':
            ms = liquid_getopt_str2mod(optarg);
            if (ms == LIQUID_MODEM_UNKNOWN) {
                fprintf(stderr,"error: %s, unknown/unsupported modulation scheme '%s'\n", argv[0], optarg);
                return 1;
            }
            break;
        default:
            exit(1);
        }
    }

    // create the modem objects
    modemq16 mod   = modemq16_create(ms);
    modemq16 demod = modemq16_create(ms);

    // ensure bits/symbol matches modem description (only
    // applicable to certain specific modems)
    unsigned int bps = modemq16_get_bps(mod);

    modemq16_print(mod);

    // open output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"m = %u;\n", bps);
    fprintf(fid,"M = %u;\n", 1<<bps);
    fprintf(fid,"c = zeros(1,M);\n");
    fprintf(fid,"i_str = cell(1,M);\n");

    unsigned int i; // modulated symbol
    unsigned int s; // demodulated symbol
    unsigned int num_symbols = 1<<bps;
    cq16_t x;
    unsigned int num_sym_errors = 0;
    unsigned int num_bit_errors = 0;

    for (i=0; i<num_symbols; i++) {
        modemq16_modulate(mod, i, &x);
        modemq16_demodulate(demod, x, &s);

        if (verbose) {
            printf("%4u > %4u : %12.8f + j*%12.8f\n",
                    i, s,
                    q16_fixed_to_float(x.real),
                    q16_fixed_to_float(x.imag));
        }

        num_sym_errors += i == s ? 0 : 1;
        num_bit_errors += count_bit_errors(i,s);

        // write symbol to output file
        fprintf(fid,"c(%3u) = %12.4e + j*%12.4e;\n", i+1, q16_fixed_to_float(x.real),
                                                          q16_fixed_to_float(x.imag));
        fprintf(fid,"i_str{%3u} = [num2str(%3u)];\n", i+1, i);
    }
    printf("num sym errors: %4u / %4u\n", num_sym_errors, num_symbols);
    printf("num bit errors: %4u / %4u\n", num_bit_errors, num_symbols*bps);

    // plot results
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(c,'o','MarkerSize',2);\n");
    fprintf(fid,"hold on;\n");
    fprintf(fid,"text(real(c)+0.02, imag(c)+0.02, i_str);\n");
    fprintf(fid,"hold off;\n");
    fprintf(fid,"axis([-1 1 -1 1]*1.6);\n");
    fprintf(fid,"axis square;\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('in phase');\n");
    fprintf(fid,"ylabel('quadrature phase');\n");

    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    modemq16_destroy(mod);
    modemq16_destroy(demod);
    return 0;
}
