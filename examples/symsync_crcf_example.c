const char __docstr__[] =
"This example demonstrates the basic principles of the symbol timing"
" recovery family of objects, specifically symsync_crcf. A set of random"
" QPSK symbols are generated and interpolated with a timing offset. The"
" resulting signal is run through the symsync_crcf object which applies a"
" matched filter and recovers timing producing a clean constellation.";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line arguments
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*, filename, "symsync_crcf_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(char*,    ftype_str,"arkaiser",'f',"filter type", liquid_argparse_firfilt);
    liquid_argparse_add(unsigned, k,           2,  'k', "filter samples per symbol", NULL);
    liquid_argparse_add(unsigned, m,           9,  'm', "filter semi-length", NULL);
    liquid_argparse_add(float,    beta,      0.3,  'b', "filter excess bandwidth factor", NULL);
    liquid_argparse_add(char*,    mod_str,  "qpsk",'M', "modulation scheme", liquid_argparse_modem);
    liquid_argparse_add(unsigned, num_filters,  32,'n', "FFT size", NULL);
    liquid_argparse_add(unsigned, num_symbols, 800,'N', "number of samples to simulate", NULL);
    liquid_argparse_add(float,    bandwidth, 0.02, 'w', "loop filter bandwidth", NULL);
    liquid_argparse_add(float,    dt,        0.50, 't', "fractional sample offset", NULL);
    liquid_argparse_parse(argc,argv);

    // validate input
    if (k < 2)
        return liquid_error(LIQUID_EICONFIG,"k (samples/symbol) must be at least 2");
    if (m < 1)
        return liquid_error(LIQUID_EICONFIG,"m (filter delay) must be greater than 0");
    if (beta <= 0.0f || beta > 1.0f)
        return liquid_error(LIQUID_EICONFIG,"beta (excess bandwidth factor) must be in (0,1]");
    if (num_filters == 0)
        return liquid_error(LIQUID_EICONFIG,"number of polyphase filters must be greater than 0");
    if (num_symbols == 0)
        return liquid_error(LIQUID_EICONFIG,"number of symbols must be greater than 0");

    // static values
    liquid_firfilt_type ftype = (liquid_firfilt_type)liquid_getopt_str2firfilt(ftype_str);

    // derived values
    unsigned int num_samples = k*num_symbols;

    LIQUID_VLA(liquid_float_complex, s, num_symbols);       // data symbols
    LIQUID_VLA(liquid_float_complex, x, num_samples);       // interpolated samples
    LIQUID_VLA(liquid_float_complex, y, num_symbols + 64);  // synchronized symbols

    // generate random symbols
    unsigned int i;
    modemcf mod = modemcf_create((modulation_scheme)liquid_getopt_str2mod(mod_str));
    modemcf_print(mod);
    for (i=0; i<num_symbols; i++) {
        unsigned int sym = modemcf_gen_rand_sym(mod);
        modemcf_modulate(mod, sym, s+i);
    }
    modemcf_destroy(mod);

    // design interpolating filter with 'dt' samples of delay
    firinterp_crcf interp = firinterp_crcf_create_prototype(ftype,k,m,beta,dt);

    // run interpolator
    firinterp_crcf_execute_block(interp, s, num_symbols, x);

    // destroy interpolator
    firinterp_crcf_destroy(interp);

    // create symbol synchronizer
    symsync_crcf sync = symsync_crcf_create_rnyquist(ftype, k, m, beta, num_filters);
    
    // set bandwidth
    symsync_crcf_set_lf_bw(sync,bandwidth);

    // execute on entire block of samples
    unsigned int ny=0;
    symsync_crcf_execute(sync, x, num_samples, y, &ny);

    // destroy synchronizer
    symsync_crcf_destroy(sync);

    // print last several symbols to screen
    printf("output symbols:\n");
    printf("  ...\n");
    for (i=ny-10; i<ny; i++)
        printf("  sym_out(%2u) = %8.4f + j*%8.4f;\n", i+1, crealf(y[i]), cimagf(y[i]));

    // export output file
    FILE* fid = fopen(filename,"w");
    fprintf(fid,"%% %s, auto-generated file\n\n", filename);
    fprintf(fid,"close all;\n");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"ny=%u;\n",ny);
    for (i=0; i<ny; i++)
        fprintf(fid,"y(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(y[i]), cimagf(y[i]));
    fprintf(fid,"i0 = 1:round(0.5*ny);\n");
    fprintf(fid,"i1 = round(0.5*ny):ny;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"hold on;\n");
    fprintf(fid,"plot(real(y(i0)),imag(y(i0)),'x','MarkerSize',4,'Color',[1 1 1]*0.7);\n");
    fprintf(fid,"plot(real(y(i1)),imag(y(i1)),'o','MarkerSize',4,'Color',[0 0.25 0.5]);\n");
    fprintf(fid,"hold off;\n");
    fprintf(fid,"axis square;\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"axis([-1 1 -1 1]*1.6);\n");
    fprintf(fid,"xlabel('In-phase');\n");
    fprintf(fid,"ylabel('Quadrature');\n");
    fprintf(fid,"legend(['first 50%%'],['last 50%%'],'location','northeast');\n");
    fclose(fid);
    printf("results written to %s.\n", filename);
    return 0;
}
