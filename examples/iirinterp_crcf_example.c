//
// iirinterp_crcf_example.c
//
// This example demonstrates the iirinterp object (IIR interpolator)
// interface.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <getopt.h>

#include "liquid.h"

#define OUTPUT_FILENAME "iirinterp_crcf_example.m"

// print usage/help message
void usage()
{
    printf("iirinterp_crcf_example:\n");
    printf("  u/h   : print usage/help\n");
    printf("  k     : samples/symbol (interp factor), k > 1, default: 4\n");
    printf("  n     : number of data symbols, default: 16\n");
}


int main(int argc, char*argv[]) {
    // options
    unsigned int k=4;                   // samples/symbol
    unsigned int num_data_symbols=16;   // number of data symbols

    int dopt;
    while ((dopt = getopt(argc,argv,"uhk:n:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h': usage();                          return 0;
        case 'k': k = atoi(optarg);                 break;
        case 'n': num_data_symbols = atoi(optarg);  break;
        default:
            exit(1);
        }
    }

    // validate options
    if (k < 2) {
        fprintf(stderr,"error: %s, interp factor must be greater than 1\n", argv[0]);
        exit(1);
    } else if (num_data_symbols < 1) {
        fprintf(stderr,"error: %s, must have at least one data symbol\n", argv[0]);
        usage();
        return 1;
    }

    // create interpolator from prototype
    liquid_iirdes_filtertype ftype  = LIQUID_IIRDES_BUTTER;
    liquid_iirdes_bandtype   btype  = LIQUID_IIRDES_LOWPASS;
    liquid_iirdes_format     format = LIQUID_IIRDES_SOS;
    unsigned int order = 8;
    float fc =  0.5f / (float)k;
    float f0 =  0.0f;
    float Ap =  0.1f;
    float As = 60.0f;
    iirinterp_crcf q = iirinterp_crcf_create_prototype(k,ftype,btype,format,order,fc,f0,Ap,As);

    // derived values
    unsigned int delay = 2; // TODO: compute actual delay
    unsigned int num_symbols = num_data_symbols + 2*delay;  // compensate for filter delay
    unsigned int num_samples = k*num_symbols;

    // generate input signal and interpolate
    float complex x[num_symbols];   // input symbols
    float complex y[num_samples];   // output samples
    unsigned int i;
    for (i=0; i<num_data_symbols; i++) {
        x[i] = (rand() % 2 ? 1.0f : -1.0f) +
               (rand() % 2 ? 1.0f : -1.0f) * _Complex_I;
    }

    // pad end of sequence with zeros
    for (i=num_data_symbols; i<num_symbols; i++)
        x[i] = 0.0f;

    // interpolate symbols
    for (i=0; i<num_symbols; i++)
        iirinterp_crcf_execute(q, x[i], &y[k*i]);

    // destroy interpolator object
    iirinterp_crcf_destroy(q);

    // print results to screen
    printf("x(t) :\n");
    for (i=0; i<num_symbols; i++)
        printf("  x(%4u) = %8.4f + j*%8.4f;\n", i, crealf(x[i]), cimagf(x[i]));

#if 0
    printf("y(t) :\n");
    for (i=0; i<num_samples; i++) {
        printf("  y(%4u) = %8.4f + j*%8.4f;", i, crealf(y[i]), cimagf(y[i]));
        if ( (i >= k*m) && ((i%k)==0))
            printf(" **\n");
        else
            printf("\n");
    }
#endif

    // 
    // export output file
    //
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"k = %u;\n", k);
    fprintf(fid,"delay = %f;\n", 1.6);
    fprintf(fid,"num_symbols = %u;\n", num_symbols);
    fprintf(fid,"num_samples = k*num_symbols;\n");
    fprintf(fid,"x = zeros(1,num_symbols);\n");
    fprintf(fid,"y = zeros(1,num_samples);\n");

    for (i=0; i<num_symbols; i++)
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));

    for (i=0; i<num_samples; i++)
        fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(k*y[i]), cimagf(k*y[i]));

    fprintf(fid,"\n\n");
    fprintf(fid,"tx = [0:(num_symbols-1)];\n");
    fprintf(fid,"ty = [0:(num_samples-1)]/k - delay;\n");
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
    printf("results written to %s.\n",OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}
