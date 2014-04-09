//
// agc_rrrf_example.c
//
// Automatic gain control example demonstrating its transient
// response on a real signal.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <getopt.h>
#include "liquid.h"

#define OUTPUT_FILENAME "agc_rrrf_example.m"

// print usage/help message
int main(int argc, char*argv[])
{
    // options
    float        gamma       = 0.01f;           // input signal level
    float        bt          = 1e-4f;           // bandwidth-time constant
    unsigned int num_samples = 2048;            // number of samples
    unsigned int d           = num_samples/32;  // print every d iterations

    unsigned int i;

    // create objects
    agc_rrrf q = agc_rrrf_create();
    agc_rrrf_set_bandwidth(q, bt);

    // squelch
    agc_rrrf_squelch_deactivate(q);

    float x[num_samples];       // input
    float y[num_samples];       // output
    float rssi[num_samples];    // received signal strength

    // print info
    printf("automatic gain control // loop bandwidth: %4.2e\n",bt);

    // generate signal
    for (i=0; i<num_samples; i++)
        x[i] = gamma * cosf(2*M_PI*0.0073f*i);

    // run agc
    agc_rrrf_reset(q);
    for (i=0; i<num_samples; i++) {
        agc_rrrf_execute(q, x[i], &y[i]);
        rssi[i] = agc_rrrf_get_rssi(q);

        if ( ((i+1)%d)==0 )
            printf("%6u : rssi = %12.8f dB\n", i, rssi[i]);
    }

    // destroy AGC object
    agc_rrrf_destroy(q);

    // 
    // export results
    //
    FILE* fid = fopen(OUTPUT_FILENAME,"w");
    if (!fid) {
        fprintf(stderr,"error: %s, could not open '%s' for writing\n", argv[0], OUTPUT_FILENAME);
        exit(1);
    }
    fprintf(fid,"%% %s: auto-generated file\n\n",OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"n = %u;\n", num_samples);

    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));
        fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));
        fprintf(fid,"rssi(%4u)  = %12.4e;\n", i+1, rssi[i]);
    }

    // plot results
    fprintf(fid,"\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"t = 0:(n-1);\n");
    fprintf(fid,"subplot(3,1,1:2);\n");
    fprintf(fid,"  plot(t,y, 'Color',[0 0.2 0.5],'LineWidth',1);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('sample index');\n");
    fprintf(fid,"  ylabel('agc output');\n");
    fprintf(fid,"subplot(3,1,3);\n");
    fprintf(fid,"  plot(t,rssi,'-','Color',[0 0.2 0.5],'LineWidth',1);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('sample index');\n");
    fprintf(fid,"  ylabel('rssi [dB]');\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

