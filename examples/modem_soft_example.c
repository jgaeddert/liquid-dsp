// Demonstrate soft demodulation of linear modulation schemes.
const char __docstr__[] = "Demonstrate soft demodulation of linear modulation schemes.";

#include <stdlib.h>
#include <stdio.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    //liquid_argparse_add(char*, filename, "modem_soft_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(char*, mod_str, "qam16", 'm', "modulation scheme", liquid_argparse_modem);
    liquid_argparse_parse(argc,argv);

    // create modem objects
    modulation_scheme ms = (modulation_scheme)liquid_getopt_str2mod(mod_str);
    modem mod = modem_create(ms);   // modulator
    modem dem = modem_create(ms);   // demodulator

    // ensure bits/symbol matches modem description (only
    // applicable to certain specific modems)
    unsigned int bps = modemcf_get_bps(mod);

    modemcf_print(mod);

    unsigned int i;         // modulated symbol
    unsigned int s_hard;    // demodulated symbol (hard)
    LIQUID_VLA(unsigned char, soft_bits, bps);
    unsigned int s_soft;    // demodulated symbol (soft, compacted)
    unsigned int num_symbols = 1<<bps;
    liquid_float_complex x;
    unsigned int num_sym_errors = 0;
    unsigned int num_bit_errors = 0;

    printf("\n");
    printf("  %-11s %-11s %-11s  : ", "input sym.", "hard demod", "soft demod");
    for (i=0; i<bps; i++)
        printf("   b[%1u]", i);
    printf("\n");

    for (i=0; i<num_symbols; i++) {
        // modulate symbol
        modemcf_modulate(mod, i, &x);

        // demodulate, including soft decision
        modemcf_demodulate_soft(dem, x, &s_hard, soft_bits);

        // re-pack soft bits to hard decision
        liquid_pack_soft_bits(soft_bits, bps, &s_soft);

        // print results
        printf("  ");
        liquid_print_bitstring(i,     bps); printf("%.*s", 12-bps, "            ");
        liquid_print_bitstring(s_hard,bps); printf("%.*s", 12-bps, "            ");
        liquid_print_bitstring(s_soft,bps); printf("%.*s", 12-bps, "            ");
        printf(" : ");
        unsigned int j;
        for (j=0; j<bps; j++)
            printf("%7u", soft_bits[j]);
        printf("\n");

        num_sym_errors += i == s_soft ? 0 : 1;
        num_bit_errors += count_bit_errors(i,s_soft);
    }
    printf("num sym errors: %4u / %4u\n", num_sym_errors, num_symbols);
    printf("num bit errors: %4u / %4u\n", num_bit_errors, num_symbols*bps);

    modemcf_destroy(mod);
    modemcf_destroy(dem);
    return 0;
}
