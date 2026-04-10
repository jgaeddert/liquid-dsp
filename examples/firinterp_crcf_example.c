const char __docstr__[] =
"This example demonstrates the firinterp object (interpolator) interface."
" Data symbols are generated and then interpolated according to a"
" finite impulse response Nyquist filter.";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*,    filename, "firinterp_crcf_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, M,         4, 'M', "interpolation factor", NULL);
    liquid_argparse_add(unsigned, m,         7, 'm', "filter semi-length", NULL);
    liquid_argparse_add(float,    As,       60, 's', "filter stop-band suppression", NULL);
    liquid_argparse_add(unsigned, num_syms, 16, 'n', "number of samples", NULL);
    liquid_argparse_parse(argc,argv);

    // validate options
    if (M < 2)
        return liquid_error(LIQUID_EICONFIG,"interp factor must be greater than 1");
    if (m < 1)
        return liquid_error(LIQUID_EICONFIG,"filter delay must be greater than 0");
    if (num_syms < 1)
        return liquid_error(LIQUID_EICONFIG,"must have at least one data symbol");

    // derived values
    unsigned int num_syms_total = num_syms + 2*m;   // total symbols (w/ delay)
    unsigned int num_samples    = M*num_syms_total; // total samples

    // create interpolator from prototype
    firinterp_crcf q = firinterp_crcf_create_kaiser(M,m,As);

    // generate input signal and interpolate
    LIQUID_VLA(liquid_float_complex, x, num_syms_total);   // input symbols
    LIQUID_VLA(liquid_float_complex, y, num_samples);   // output samples
    unsigned int i;
    for (i=0; i<num_syms; i++) {
        x[i] = (rand() % 2 ? 1.0f : -1.0f) +
               (rand() % 2 ? 1.0f : -1.0f) * _Complex_I;
    }

    // pad end of sequence with zeros
    for (i=num_syms; i<num_syms_total; i++)
        x[i] = 0.0f;

    // interpolate symbols
    for (i=0; i<num_syms_total; i++)
        firinterp_crcf_execute(q, x[i], &y[M*i]);

    // destroy interpolator object
    firinterp_crcf_destroy(q);

    // print results to screen
    printf("x(t) :\n");
    for (i=0; i<num_syms_total; i++)
        printf("  x(%4u) = %8.4f + j*%8.4f;\n", i, crealf(x[i]), cimagf(x[i]));

    printf("y(t) :\n");
    for (i=0; i<num_samples; i++) {
        printf("  y(%4u) = %8.4f + j*%8.4f;", i, crealf(y[i]), cimagf(y[i]));
        if ( (i >= M*m) && ((i%M)==0))
            printf(" **\n");
        else
            printf("\n");
    }

    // export output file
    FILE * fid = fopen(filename,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", filename);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"M = %u;\n", M);
    fprintf(fid,"m = %u;\n", m);
    fprintf(fid,"num_syms_total = %u;\n", num_syms_total);
    fprintf(fid,"num_samples = M*num_syms_total;\n");
    fprintf(fid,"x = zeros(1,num_syms_total);\n");
    fprintf(fid,"y = zeros(1,num_samples);\n");

    for (i=0; i<num_syms_total; i++)
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));

    for (i=0; i<num_samples; i++)
        fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));

    fprintf(fid,"\n\n");
    fprintf(fid,"tx = [0:(num_syms_total-1)];\n");
    fprintf(fid,"ty = [0:(num_samples-1)]/M - m;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"    plot(ty,real(y),'-',tx,real(x),'s');\n");
    fprintf(fid,"    xlabel('time');\n");
    fprintf(fid,"    ylabel('real');\n");
    fprintf(fid,"    grid on;\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"    plot(ty,imag(y),'-',tx,imag(x),'s');\n");
    fprintf(fid,"    xlabel('time');\n");
    fprintf(fid,"    ylabel('imag');\n");
    fprintf(fid,"    grid on;\n");

    fclose(fid);
    printf("results written to %s.\n",filename);

    printf("done.\n");
    return 0;
}
