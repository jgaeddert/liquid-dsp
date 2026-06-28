char __docstr__[] =
"This example demonstrates the functionality of the qdsync object on"
" continuous phase frequency-shift keying (cpfsk) signals.";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <math.h>
#include <time.h>
#include "liquid.h"
#include "liquid.argparse.h"

// synchronization callback, return 0:continue, 1:reset
int callback(float complex * _buf,
             unsigned int    _buf_len,
             void *          _context)
{
    printf("callback got %u samples\n", _buf_len);
    unsigned int i;
    for (i=0; i<_buf_len; i++)
        fprintf((FILE*)_context, "y(end+1) = %12.8f + %12.8fj;\n", crealf(_buf[i]), cimagf(_buf[i]));
    return 0;
}

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "qdsync_cccf_cpfsk_example.m",'o', "output filename", NULL);
    liquid_argparse_add(unsigned, num_symbols,    80,   'n', "number of sync symbols", NULL);
    liquid_argparse_add(float,    mod_index,    0.5f,   'H', "modulation index (H=1/2 for MSK)", NULL);
    liquid_argparse_add(unsigned, k,               2,   'k', "samples/symbol", NULL);
    liquid_argparse_add(unsigned, m,               7,   'm', "filter delay [symbols]", NULL);
    liquid_argparse_add(float,    beta,         0.3f,   'b', "excess bandwidth factor", NULL);
    liquid_argparse_add(char *,   filter,      "square",'t', "filter type: square, rcos-full, rcos-half, gmsk", NULL);
    //liquid_argparse_add(float,    tau,          -0.3f,  'T', "fractional sample timing offset", NULL);
    //liquid_argparse_add(float,    dphi,         -0.01f, 'F', "carrier frequency offset", NULL);
    //liquid_argparse_add(float,    phi,           0.5f,  'P', "carrier phase offset", NULL);
    liquid_argparse_add(float,    nstd,          0.01f, '0', "noise standard deviation", NULL);
    liquid_argparse_parse(argc,argv);

    // create sync object
    int filter_type = LIQUID_CPFSK_SQUARE;
    if (strcmp(filter,"square")==0) {
        filter_type = LIQUID_CPFSK_SQUARE;
    } else if (strcmp(filter,"rcos-full")==0) {
        filter_type = LIQUID_CPFSK_RCOS_FULL;
    } else if (strcmp(filter,"rcos-half")==0) {
        filter_type = LIQUID_CPFSK_RCOS_PARTIAL;
    } else if (strcmp(filter,"gmsk")==0) {
        filter_type = LIQUID_CPFSK_GMSK;
    } else {
        return liquid_error(LIQUID_EICONFIG,"unknown filter type '%s'", filter);
    }

    // open file for storing results
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all; close all; y=[];\n");

    // generate synchronization sequence (CPFSK bits)
    unsigned char syms[num_symbols];
    unsigned int i;
    for (i=0; i<num_symbols; i++)
        syms[i] = rand() & 1;
    qdsync_cccf q = qdsync_cccf_create_cpfsk(syms, num_symbols, LIQUID_FIRFILT_ARKAISER,
        1, mod_index, k, m, beta, filter_type, callback, fid);
    qdsync_cccf_print(q);

    // create CPFSK modulator
    cpfskmod mod = cpfskmod_create(1, mod_index, k, m, beta, filter_type);

    // run signal through sync object
    float complex buf[k];
    for (i=0; i<10*num_symbols; i++) {
        // generate symbol
        unsigned int s = i < num_symbols ? syms[i] : (rand() & 1);

        // modulate symbol
        cpfskmod_modulate(mod, s, buf);

        // add noise
        unsigned int j;
        for (j=0; j<k; j++)
            buf[j] += nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;

        // run through synchronizer
        qdsync_cccf_execute(q, buf, k);
    }
    qdsync_cccf_destroy(q);
    cpfskmod_destroy(mod);

    // export results
    fprintf(fid,"sequence_len = %u;\n", num_symbols);
    fprintf(fid,"figure('color','white','position',[100 100 720 720]);\n");
    fprintf(fid,"hold on;\n");
    fprintf(fid,"  plot(real(y(1:sequence_len)),      imag(y(1:sequence_len)),      '.','MarkerSize',6,'Color',[0 0.2 0.5]);\n");
    fprintf(fid,"  plot(real(y((sequence_len+1):end)),imag(y((sequence_len+1):end)),'.','MarkerSize',6,'Color',[0 0.5 0.2]);\n");
    fprintf(fid,"hold off;\n");
    fprintf(fid,"legend('Preamble','Payload');\n");
    fprintf(fid,"grid on; xlabel('real'); ylabel('imag');\n");
    fclose(fid);
    printf("results written to '%s'\n", filename);
    return 0;
}
