const char __docstr__[] =
"This example demonstrates the digital modulator/demodulator"
" (modem) object.  Data symbols are modulated into complex"
" samples which are then demodulated without noise or phase"
" offsets.  The user may select the modulation scheme via"
" the command-line interface.";

#include <math.h>
#ifndef _MSC_VER
#include <complex.h>
#endif
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "modem_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(char*, mod_scheme, "qpsk", 'm', "modulation scheme", liquid_argparse_modem);
    liquid_argparse_parse(argc,argv);

    // create the modem objects
    modulation_scheme ms = (modulation_scheme)liquid_getopt_str2mod(mod_scheme);
    modemcf mod   = modemcf_create(ms);
    modemcf demod = modemcf_create(ms);

    // ensure bits/symbol matches modem description (only
    // applicable to certain specific modems)
    unsigned int bps = modemcf_get_bps(mod);

    modemcf_print(mod);

    // open output file
    FILE*fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"m = %u;\n", bps);
    fprintf(fid,"M = %u;\n", 1<<bps);
    fprintf(fid,"c = zeros(1,M);\n");
    fprintf(fid,"i_str = cell(1,M);\n");

    unsigned int i; // modulated symbol
    unsigned int s; // demodulated symbol
    unsigned int num_symbols = 1<<bps;
    liquid_float_complex x;
    unsigned int num_sym_errors = 0;
    unsigned int num_bit_errors = 0;

    for (i=0; i<num_symbols; i++) {
        modemcf_modulate(mod, i, &x);
        modemcf_demodulate(demod, x, &s);

        printf("%4u : %12.8f + j*%12.8f\n", i, crealf(x), cimagf(x));

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
    printf("results written to %s.\n", filename);

    modemcf_destroy(mod);
    modemcf_destroy(demod);
    return 0;
}
