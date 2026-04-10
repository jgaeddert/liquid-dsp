const char __docstr__[] =
"This example demonstrates the functionality of the qdsync object to"
" detect and synchronize an arbitrary signal in time in the presence of noise,"
" carrier frequency/phase offsets, and fractional-sample timing offsets."
" offsets.";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <math.h>
#include <time.h>
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

// synchronization callback, return 0:continue, 1:reset
int callback(liquid_float_complex * _buf,
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
    liquid_argparse_add(char*, filename, "qdsync_cccf_example.m",'o', "output filename", NULL);
    liquid_argparse_add(unsigned, sequence_len,   80,   'n', "number of sync symbols", NULL);
    liquid_argparse_add(unsigned, k,               2,   'k', "samples/symbol", NULL);
    liquid_argparse_add(unsigned, m,               7,   'm', "filter delay [symbols]", NULL);
    liquid_argparse_add(float,    beta,         0.3f,   'b', "excess bandwidth factor", NULL);
    liquid_argparse_add(char*,    ftype_str, "arkaiser",'t', "filter type", liquid_argparse_firfilt);
    //liquid_argparse_add(float,    tau,          -0.3f,  'T', "fractional sample timing offset", NULL);
    //liquid_argparse_add(float,    dphi,         -0.01f, 'F', "carrier frequency offset", NULL);
    //liquid_argparse_add(float,    phi,           0.5f,  'P', "carrier phase offset", NULL);
    liquid_argparse_add(float,    nstd,          0.01f, '0', "noise standard deviation", NULL);
    liquid_argparse_parse(argc,argv);

    // generate synchronization sequence (QPSK symbols)
    LIQUID_VLA(liquid_float_complex, seq, sequence_len);
    unsigned int i;
    for (i=0; i<sequence_len ; i++) {
        seq[i] = (rand() % 2 ? 1.0f : -1.0f) * M_SQRT1_2 +
                 (rand() % 2 ? 1.0f : -1.0f) * M_SQRT1_2 * _Complex_I;
    }

    // open file for storing results
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n", filename);
    fprintf(fid,"clear all; close all; y=[];\n");

    // create sync object
    int ftype = liquid_getopt_str2firfilt(ftype_str);
    qdsync_cccf q = qdsync_cccf_create_linear(seq, sequence_len, ftype, k, m, beta, callback, fid);
    qdsync_cccf_print(q);

    // create interpolator
    firinterp_crcf interp = firinterp_crcf_create_prototype(ftype,k,m,beta,0);

    // run signal through sync object
    LIQUID_VLA(liquid_float_complex, buf, k);
    for (i=0; i<10*sequence_len; i++) {
        // generate random symbol
        liquid_float_complex s = i < sequence_len ? seq[i] : (rand() & 1 ? 1.0f : -1.0f);

        // interpolate symbol
        firinterp_crcf_execute(interp, s, buf);

        // add noise
        unsigned int j;
        for (j=0; j<k; j++)
            buf[j] += nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;

        // run through synchronizer
        qdsync_cccf_execute(q, buf, k);
    }
    qdsync_cccf_destroy(q);
    firinterp_crcf_destroy(interp);

    // export results
    fprintf(fid,"sequence_len = %u;\n", sequence_len);
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
