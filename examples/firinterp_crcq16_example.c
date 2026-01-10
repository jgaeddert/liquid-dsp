char __docstr__[] =
"This example demonstrates the firinterp object (FIR interpolator)"
" interface with fixed-point precision";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*,    filename, "firinterp_crcq16_example.m", 'o', "output filename", NULL);
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
    firinterp_crcq16 q = firinterp_crcq16_create_kaiser(M,m,As);

    // generate input signal and interpolate
    cq16_t x[num_syms_total]; // input samples
    cq16_t y[num_samples]; // output samples
    unsigned int i;
    for (i=0; i<num_syms; i++) {
        x[i].real = (rand() % 2 ? q16_one : -q16_one);
        x[i].imag = (rand() % 2 ? q16_one : -q16_one);
    }

    // pad end of sequence with zeros
    for (i=num_syms; i<num_syms_total; i++)
        x[i] = cq16_zero;

    // interpolate symbols
    for (i=0; i<num_syms_total; i++)
        firinterp_crcq16_execute(q, x[i], &y[M*i]);

    // destroy interpolator object
    firinterp_crcq16_destroy(q);

    // print results to screen
    printf("x(t) :\n");
    for (i=0; i<num_syms_total; i++)
    {
        printf("  x(%4u) = %8.4f + j*%8.4f;\n", i,
            q16_fixed_to_float(x[i].real),
            q16_fixed_to_float(x[i].imag));
    }

    printf("y(t) :\n");
    for (i=0; i<num_samples; i++) {
        printf("  y(%4u) = %8.4f + j*%8.4f;", i,
            q16_fixed_to_float(y[i].real),
            q16_fixed_to_float(y[i].imag));
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
    {
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n",
            i+1,
            q16_fixed_to_float(x[i].real),
            q16_fixed_to_float(x[i].imag));
    }

    for (i=0; i<num_samples; i++)
    {
        fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n",
                i+1,
                q16_fixed_to_float(y[i].real),
                q16_fixed_to_float(y[i].imag));
    }

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

