//
// firfarrow_rrrf_sine_example.c
//
// Demonstrates the functionality of the finite impulse response Farrow
// filter for arbitrary fractional sample group delay.
//

#include <stdio.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "firfarrow_rrrf_sine_example.m"

int main() {
    // options
    unsigned int h_len=19;          // filter length
    unsigned int p=5;               // polynomial order
    float fc=0.45f;                 // filter cutoff
    float As=60.0f;                 // stop-band attenuation [dB]
    float mu=0.1f;                  // fractional sample delay
    unsigned int num_samples=32;    // number of samples to evaluate
    float f0=0.125f;                // sine wave frequency

    // data arrays
    float x[num_samples];           // input data array
    float y[num_samples];           // output data array

    // create and initialize Farrow filter object
    firfarrow_rrrf f = firfarrow_rrrf_create(h_len, p, fc, As);
    firfarrow_rrrf_set_delay(f, mu);

    unsigned int i;

    // generate input data
    for (i=0; i<num_samples; i++)
        x[i] = sinf(2*M_PI*f0*i);

    // push input through filter
    for (i=0; i<num_samples; i++) {
        firfarrow_rrrf_push(f, x[i]);
        firfarrow_rrrf_execute(f, &y[i]);
    }


    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"h_len = %u;\n", h_len);
    fprintf(fid,"mu = %f;\n", mu);
    fprintf(fid,"num_samples = %u;\n", num_samples);

    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%4u) = %12.4e; y(%4u) = %12.4e;\n", i+1, x[i], i+1, y[i]);
    }

    // plot the results
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"t=0:(num_samples-1);\n");
    fprintf(fid,"plot(t,x,'-x',t - (h_len-1)/2 + mu,y,'-x');\n");
    fprintf(fid,"legend('x','y',0);\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    firfarrow_rrrf_destroy(f);

    printf("done.\n");
    return 0;
}

