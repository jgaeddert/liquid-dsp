const char __docstr__[] =
"Demonstrates the functionality of the finite impulse response Farrow"
" filter for arbitrary fractional sample group delay.";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char*argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*,    filename, "firfarrow_rrrf_example.m", 'o', "output filename", NULL);
    liquid_argparse_add(unsigned, h_len,         19, 'H', "filter length", NULL);
    liquid_argparse_add(unsigned, p,              5, 'p', "polynomial order", NULL);
    liquid_argparse_add(float,    fc,          0.45, 'f', "filter cutoff", NULL);
    liquid_argparse_add(float,    As,          60.0, 's', "stop-band attenuation [dB]", NULL);
    liquid_argparse_add(float,    mu,           0.1, 'u', "fractional sample delay", NULL);
    liquid_argparse_add(unsigned, num_samples,   60, 'n', "number of samples to evaluate", NULL);
    liquid_argparse_parse(argc,argv);

    // data arrays
    LIQUID_VLA(float, x, num_samples);   // input data array
    LIQUID_VLA(float, y, num_samples);   // output data array

    // create and initialize Farrow filter object
    firfarrow_rrrf f = firfarrow_rrrf_create(h_len, p, fc, As);
    firfarrow_rrrf_set_delay(f, mu);

    // create low-pass filter for input signal
    iirfilt_rrrf lowpass = iirfilt_rrrf_create_lowpass(3, 0.1f);

    // push input through filter
    unsigned int i;
    for (i=0; i<num_samples; i++) {
        // generate input (filtered noise)
        iirfilt_rrrf_execute(lowpass, randnf(), &x[i]);

        // push result through Farrow filter to add slight delay
        firfarrow_rrrf_push(f, x[i]);
        firfarrow_rrrf_execute(f, &y[i]);
    }

    // destroy Farrow and low-pass filter objects
    firfarrow_rrrf_destroy(f);
    iirfilt_rrrf_destroy(lowpass);

    FILE*fid = fopen(filename,"w");
    fprintf(fid,"%% %s : auto-generated file\n\n", filename);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"h_len = %u;\n", h_len);
    fprintf(fid,"mu = %f;\n", mu);
    fprintf(fid,"num_samples = %u;\n", num_samples);

    for (i=0; i<num_samples; i++)
        fprintf(fid,"x(%4u) = %12.4e; y(%4u) = %12.4e;\n", i+1, x[i], i+1, y[i]);

    // plot the results
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"tx = 0:(num_samples-1);     %% input time scale\n");
    fprintf(fid,"ty = tx - (h_len-1)/2 + mu; %% output time scale\n");
    fprintf(fid,"plot(tx, x,'-s','MarkerSize',3, ...\n");
    fprintf(fid,"     ty, y,'-x','MarkerSize',3);\n");
    fprintf(fid,"legend('input','output');\n");

    fclose(fid);
    printf("results written to %s\n", filename);

    printf("done.\n");
    return 0;
}

