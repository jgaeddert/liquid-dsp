// file: modem_test.c
//
// Tests simple modulation/demodulation without noise or phase
// offset, counting the number of resulting symbol errors.
//
// Complile and run:
//   $ gcc modem_test.c -lliquid -o modemtest
//   $ ./modemtest

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include "liquid.h"

#define OUTPUT_FILENAME "modem_example.m"

// print usage/help message
void usage()
{
    printf("modem_example [options]\n");
    printf("  u/h   : print usage\n");
    printf("  p     : modulation depth (default 2 bits/symbol)\n");
    printf("  m     : modulation scheme (psk default)\n");
    // print all available MOD schemes
    unsigned int i;
    for (i=0; i<LIQUID_NUM_MOD_SCHEMES; i++)
        printf("          %s\n", modulation_scheme_str[i]);
}


int main(int argc, char*argv[]) {
    // create mod/demod objects
    unsigned int bps=2;
    modulation_scheme ms = MOD_PSK;

    int dopt;
    while ((dopt = getopt(argc,argv,"uhm:p:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h':
            usage();
            return 0;
        case 'm':
            ms = liquid_getopt_str2mod(optarg);
            if (ms == MOD_UNKNOWN) {
                fprintf(stderr,"error: modem_example, unknown/unsupported modulation scheme \"%s\"\n", optarg);
                return 1;
            }
            break;
        case 'p':
            bps = atoi(optarg);
            break;
        default:
            fprintf(stderr,"error: modem_example, unknown option\n");
            usage();
            return 1;
        }
    }
    modem mod = modem_create(ms, bps);
    modem demod = modem_create(ms, bps);

    modem_print(mod);

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
    float complex x;
    unsigned int num_sym_errors = 0;
    unsigned int num_bit_errors = 0;

    for (i=0; i<num_symbols; i++) {
        modem_modulate(mod, i, &x);
        modem_demodulate(demod, x, &s);

        num_sym_errors += i == s ? 0 : 1;
        num_bit_errors += count_bit_errors(i,s);

        // write symbol to output file
        fprintf(fid,"c(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(x), cimagf(x));
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

    modem_destroy(mod);
    modem_destroy(demod);
    return 0;
}
