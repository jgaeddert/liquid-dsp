// 
// modem_soft_example.c
//
// This example demonstates soft demodulation of linear
// modulation schemes.
//

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include "liquid.h"

#define OUTPUT_FILENAME "modem_soft_example.m"

// print usage/help message
void usage()
{
    printf("modem_soft_example [options]\n");
    printf("  u/h   : print usage\n");
    printf("  p     : modulation depth (default 2 bits/symbol)\n");
    printf("  m     : modulation scheme (psk default)\n");
    liquid_print_modulation_schemes();
}

// print a string of bits to the standard output
void print_bitstring(unsigned int _x,
                     unsigned int _n)
{
    unsigned int i;
    for (i=0; i<_n; i++)
        printf("%1u", (_x >> (_n-i-1)) & 1);
}


int main(int argc, char*argv[])
{
    // create mod/demod objects
    unsigned int bps=2;
    modulation_scheme ms = LIQUID_MODEM_PSK;

    int dopt;
    while ((dopt = getopt(argc,argv,"uhm:p:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h':
            usage();
            return 0;
        case 'm':
            ms = liquid_getopt_str2mod(optarg);
            if (ms == LIQUID_MODEM_UNKNOWN) {
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

    // create the modem objects
    modem mod   = modem_create(ms, bps);
    modem demod = modem_create(ms, bps);

    // ensure bits/symbol matches modem description (only
    // applicable to certain specific modems)
    bps = modem_get_bps(mod);

    modem_print(mod);

    unsigned int i; // modulated symbol
    unsigned int s; // demodulated symbol
    unsigned char soft_bits[bps];
    unsigned int num_symbols = 1<<bps;
    float complex x;
    unsigned int num_sym_errors = 0;
    unsigned int num_bit_errors = 0;

    for (i=0; i<num_symbols; i++) {
        // modulate symbol
        modem_modulate(mod, i, &x);

        // demodulate, including soft decision
        modem_demodulate_soft(demod, x, &s, soft_bits);

        // print results
        printf("  ");
        print_bitstring(i,bps);
        printf(" : ");
        print_bitstring(s,bps);
        printf(" : ");
        unsigned int j;
        for (j=0; j<bps; j++)
            printf("%6u", soft_bits[j]);
        printf("\n");

        num_sym_errors += i == s ? 0 : 1;
        num_bit_errors += count_bit_errors(i,s);
    }
    printf("num sym errors: %4u / %4u\n", num_sym_errors, num_symbols);
    printf("num bit errors: %4u / %4u\n", num_bit_errors, num_symbols*bps);

    modem_destroy(mod);
    modem_destroy(demod);
    return 0;
}
