//
// agc_crcf_example.c
//
// Automatic gain control example demonstrating its transient
// response.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <getopt.h>
#include "liquid.h"

#define OUTPUT_FILENAME "agc_crcf_example.m"

// print usage/help message
void usage()
{
    printf("agc_example [options]\n");
    printf("  h     : print usage\n");
    printf("  n     : number of samples, n >=100, default: 2048\n");
    printf("  b     : AGC bandwidth,     b >=  0, default: 0.01\n");
}


int main(int argc, char*argv[])
{
    // options
    float        bt          = 0.01f;   // agc loop bandwidth
    unsigned int num_samples = 2048;    // number of samples

    int dopt;
    while((dopt = getopt(argc,argv,"hn:N:s:b:")) != EOF){
        switch (dopt) {
        case 'h': usage();                      return 0;
        case 'n': num_samples = atoi(optarg);   break;
        case 'b': bt          = atof(optarg);   break;
        default:
            exit(1);
        }
    }

    // validate input
    if (bt < 0.0f) {
        fprintf(stderr,"error: %s, bandwidth must be positive\n", argv[0]);
        exit(1);
    } else if (num_samples < 100) {
        fprintf(stderr,"error: %s, must have at least 100 samples\n", argv[0]);
        exit(1);
    }
    
    unsigned int i;

    // create objects
    agc_crcf q = agc_crcf_create();
    agc_crcf_set_bandwidth(q, bt);

    float complex x[num_samples];   // input
    float complex y[num_samples];   // output
    float rssi[num_samples];        // received signal strength

    // print info
    printf("automatic gain control // loop bandwidth: %4.2e\n",bt);

    // generate signal
    for (i=0; i<num_samples; i++)
        x[i] = 0.001 * cexpf(_Complex_I*2*M_PI*0.093f*i);

    // run agc
    for (i=0; i<num_samples; i++) {
        agc_crcf_execute(q, x[i], &y[i]);
        rssi[i] = agc_crcf_get_rssi(q);
    }

    // destroy AGC object
    agc_crcf_destroy(q);

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
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(t,real(x), t,imag(x), '-','LineWidth',1);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('sample index');\n");
    fprintf(fid,"  ylabel('input');\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(t,real(y), t,imag(y), '-','LineWidth',1);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('sample index');\n");
    fprintf(fid,"  ylabel('output');\n");

    fprintf(fid,"\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,rssi,'-','LineWidth',1);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('sample index');\n");
    fprintf(fid,"ylabel('rssi [dB]');\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

