//
// interp_crcf_example.c
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "interp_crcf_example.m"

int main() {
    // options
    unsigned int k=4;                   // samples/symbol
    unsigned int m=3;                   // filter delay
    float beta = 0.3f;                  // filter excess bandwidth
    unsigned int num_data_symbols=16;   // number of data symbols

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
    float theta;
    for (i=0; i<num_data_symbols; i++) {
        theta = M_PI * (0.5f*(rand()%4) + 0.25f);
        x[i] = cexpf(_Complex_I * theta);
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
