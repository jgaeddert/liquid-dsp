//
// firfarrow_rrrf_example.c
//
// Demonstrates the functionality of the finite impulse response Farrow
// filter for arbitrary fractional sample group delay.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <getopt.h>

#include "liquid.h"

#define OUTPUT_FILENAME "firfarrow_rrrf_example.m"

// print usage/help message
void usage()
{
    printf("firfarrow_rrrf_example [options]\n");
    printf("  h     : print help\n");
    printf("  v     : verbose/quiet\n");
    printf("  t     : fractional sample offset, t in [-0.5,0.5], default: 0.2\n");
}

int main(int argc, char*argv[])
{
    // options
    unsigned int h_len=19;          // filter length
    unsigned int p=5;               // polynomial order
    float fc=0.45f;                 // filter cutoff
    float As=60.0f;                 // stop-band attenuation [dB]
    float mu=0.1f;                  // fractional sample delay
    unsigned int num_samples=60;    // number of samples to evaluate
    int verbose = 0;                // verbose output?

    int dopt;
    while ((dopt = getopt(argc,argv,"hvt:")) != EOF) {
        switch (dopt) {
        case 'h': usage();              return 0;
        case 'v': verbose = 1;          break;
        case 't': mu = atof(optarg);    break;
        default:
            exit(1);
        }
    }

    // data arrays
    float x[num_samples];           // input data array
    float y[num_samples];           // output data array

    // create and initialize Farrow filter object
    firfarrow_rrrf f = firfarrow_rrrf_create(h_len, p, fc, As);
    firfarrow_rrrf_set_delay(f, mu);

    unsigned int i;

    // generate input (filtered noise)
    float hx[21];
    liquid_firdes_kaiser(15, 0.1, 60, 0, hx);
    firfilt_rrrf fx = firfilt_rrrf_create(hx, 15);
    for (i=0; i<num_samples; i++) {
        firfilt_rrrf_push(fx, i < 40 ? randnf() : 0.0f);
        firfilt_rrrf_execute(fx, &x[i]);
    }
    firfilt_rrrf_destroy(fx);

    // push input through filter
    for (i=0; i<num_samples; i++) {
        firfarrow_rrrf_push(f, x[i]);
        firfarrow_rrrf_execute(f, &y[i]);
    }

    // destroy Farrow filter object
    firfarrow_rrrf_destroy(f);

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
    fprintf(fid,"tx = 0:(num_samples-1);     %% input time scale\n");
    fprintf(fid,"ty = tx - (h_len-1)/2 + mu; %% output time scale\n");
    fprintf(fid,"plot(tx, x,'-s','MarkerSize',3, ...\n");
    fprintf(fid,"     ty, y,'-s','MarkerSize',3);\n");
    fprintf(fid,"legend('input','output',0);\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

