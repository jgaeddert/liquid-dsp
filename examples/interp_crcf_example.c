//
// interp_crcf_example.c
//
// This example demonstrates the interp object (interpolator) interface.
// Data symbols are generated and then interpolated according to a
// finite impulse response Nyquist filter.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <getopt.h>

#include "liquid.h"

#define OUTPUT_FILENAME "interp_crcf_example.m"

// print usage/help message
void usage()
{
    printf("interp_crcf_example:\n");
    printf("  u/h   : print usage/help\n");
    printf("  k     : samples/symbol (interp factor), k > 1, default: 4\n");
    printf("  m     : filter delay (symbols), m > 0, default: 3\n");
    printf("  b     : beta, excess bandwidth factor, 0 < beta < 1, default: 0.3\n");
    printf("  n     : number of data symbols, default: 16\n");
}


int main(int argc, char*argv[]) {
    // options
    unsigned int k=4;                   // samples/symbol
    unsigned int m=3;                   // filter delay
    float beta = 0.3f;                  // filter excess bandwidth
    unsigned int num_data_symbols=16;   // number of data symbols

    int dopt;
    while ((dopt = getopt(argc,argv,"uhk:m:b:n:")) != EOF) {
        switch (dopt) {
        case 'u':
        case 'h': usage();                          return 0;
        case 'k': k = atoi(optarg);                 break;
        case 'm': m = atoi(optarg);                 break;
        case 'b': beta = atof(optarg);              break;
        case 'n': num_data_symbols = atoi(optarg);  break;
        default:
            fprintf(stderr,"error: %s, unknown option\n", argv[0]);
            usage();
            return 1;
        }
    }

    // validate options
    if (k < 2) {
        fprintf(stderr,"error: %s, interp factor must be greater than 1\n", argv[0]);
        usage();
        return 1;
    } else if (m < 1) {
        fprintf(stderr,"error: %s, filter delay must be greater than 0\n", argv[0]);
        usage();
        return 1;
    } else if (beta <= 0.0 || beta > 1.0f) {
        fprintf(stderr,"error: %s, beta (excess bandwidth factor) must be in (0,1]\n", argv[0]);
        usage();
        return 1;
    } else if (num_data_symbols < 1) {
        fprintf(stderr,"error: %s, must have at least one data symbol\n", argv[0]);
        usage();
        return 1;
    }

    // derived values
    unsigned int h_len = 2*k*m+1;
    unsigned int num_symbols = num_data_symbols + 2*m;
    unsigned int num_samples = k*num_symbols;

    // design filter and create interpolator
    float h[h_len];
    design_rcos_filter(k,m,beta,0.0f,h);
    interp_crcf q = interp_crcf_create(k,h,h_len);

    // generate input signal and interpolate
    float complex x[num_symbols];
    float complex y[num_samples];
    unsigned int i;
    for (i=0; i<num_data_symbols; i++) {
        x[i] = (rand() % 2 ? 1.0f : -1.0f) +
               (rand() % 2 ? 1.0f : -1.0f) * _Complex_I;
    }
    for ( ; i<num_symbols; i++)
        x[i] = 0.0f;

    unsigned int ny=0;
    for (i=0; i<num_symbols; i++) {
        interp_crcf_execute(q, x[i], &y[ny]);
        ny += k;
    }

    // print results to screen
    printf("h(t) :\n");
    for (i=0; i<h_len; i++)
        printf("  h(%2u) = %8.4f;\n", i+1, h[i]);

    printf("x(t) :\n");
    for (i=0; i<num_symbols; i++)
        printf("  x(%2u) = %8.4f + j*%8.4f;\n", i, crealf(x[i]), cimagf(x[i]));

    printf("y(t) :\n");
    for (i=0; i<num_samples; i++) {
        printf("  y(%2u) = %8.4f + j*%8.4f;", i, crealf(y[i]), cimagf(y[i]));
        if ( (i >= k*m) && ((i%k)==0))
            printf(" **\n");
        else
            printf("\n");
    }

    // open output file
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"k = %u;\n", k);
    fprintf(fid,"m = %u;\n", m);
    fprintf(fid,"h_len=%u;\n",h_len);
    fprintf(fid,"num_symbols = %u;\n", num_symbols);
    fprintf(fid,"num_samples = k*num_symbols;\n");
    fprintf(fid,"h = zeros(1,h_len);\n");
    fprintf(fid,"x = zeros(1,num_symbols);\n");
    fprintf(fid,"y = zeros(1,num_samples);\n");

    for (i=0; i<h_len; i++)
        fprintf(fid,"h(%4u) = %12.4e;\n", i+1, h[i]);

    for (i=0; i<num_symbols; i++)
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));

    for (i=0; i<num_samples; i++)
        fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));

    fprintf(fid,"\n\n");
    fprintf(fid,"tx = [0:(num_symbols-1)];\n");
    fprintf(fid,"ty = [0:(num_samples-1)]/k - m;\n");
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

    interp_crcf_destroy(q);
    printf("done.\n");
    return 0;
}
