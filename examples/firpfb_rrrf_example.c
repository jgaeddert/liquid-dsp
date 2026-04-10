const char __docstr__[] =
"This example demonstrates using a poly-phase filter-bank as interpolator";

#include <stdlib.h>
#include <stdio.h>
#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char* argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*,    filename, "firpfb_rrrf_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, M,           16, 'M', "interpolation factor", NULL);
    liquid_argparse_add(unsigned, m,            4, 'm', "filter delay (input samples)", NULL);
    liquid_argparse_add(unsigned, num_samples, 40, 'n', "number of samples to generate", NULL);
    liquid_argparse_parse(argc,argv);

    // create object
    firpfb_rrrf pfb = firpfb_rrrf_create_default(M, m);
    firpfb_rrrf_print(pfb);

    // generate and interpolate signal (windowed sinc pulse)
    LIQUID_VLA(float, buf_0, num_samples);
    LIQUID_VLA(float, buf_1, M*num_samples);
    unsigned int i, j;
    for (i=0; i<num_samples; i++) {
        // generate input random +1/-1 sequence
        buf_0[i] = rand() & 1 ? 1. : -1.;

        // push sample into filter bank
        firpfb_rrrf_push(pfb, buf_0[i]);

        // interpolate result (one output per branch)
        for (j=0; j<M; j++)
            firpfb_rrrf_execute(pfb, j, buf_1 + i*M + j);
    }

    // clean up objects
    firpfb_rrrf_destroy(pfb);

    // output to file
    FILE*fid = fopen(filename,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", filename);
    fprintf(fid,"clear all; close all;\n\n");
    fprintf(fid,"M = %u; m = %u; num_samples = %u\n", M, m, num_samples);
    fprintf(fid,"x = zeros(1,  num_samples);\n");
    fprintf(fid,"y = zeros(1,M*num_samples);\n");
    for (i=0; i<  num_samples; i++) { fprintf(fid,"x(%3u) = %12.4e;\n", i+1, buf_0[i]); }
    for (i=0; i<M*num_samples; i++) { fprintf(fid,"y(%3u) = %12.4e;\n", i+1, buf_1[i]); }
    fprintf(fid,"tx = [0:(  num_samples-1)];\n");
    fprintf(fid,"ty = [0:(M*num_samples-1)]/M - m;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(ty,y,'-k',tx,x,'ob','MarkerSize',2);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('Input Sample Index');\n");
    fprintf(fid,"ylabel('Signal');\n");
    fprintf(fid,"legend('Output','Input');\n");
    fclose(fid);
    printf("results written to %s\n", filename);
    return 0;
}
