//
// lpc_example.c
//

#include <stdio.h>
#include <math.h>
#include <complex.h>

#include "liquid.h"

#define OUTPUT_FILENAME "lpc_example.m"

int main() {
    // options
    unsigned int n = 200;   // input sequence length
    unsigned int p = 4;     // prediction filter order

    // derived values
#if 0
    float b[4] = {1.0f, 0.0f, 0.0f, 0.0f};
    //float a[4] = {1.0f, 0.5f, 0.4f, 0.3f};
    float a[4] = {1.0f, 0.0f, 0.0f, 0.9f};
#else
    // ./examples/iirdes_example -t butter -n3 -otf -f0.2
    float b[4] = {0.01809893, 0.05429679, 0.05429679, 0.01809893};
    float a[4] = {1.00000000, -1.76004195, 1.18289328, -0.27805993};
#endif

    // create filter object
    iirfilt_rrrf f = iirfilt_rrrf_create(b,4, a,4);
    iirfilt_rrrf_print(f);

    unsigned int i;

    // allocate memory for data arrays
    float x[n];         // input noise sequence
    float y[n];         // output filtered noise sequence
    float a_hat[p+1];   // lpc output
    float g_hat[p+1];   // lpc output

    // generate input noise signal
    for (i=0; i<n; i++) {
        x[i] = randnf();
        //x[i] = ( (i%10) == 0 ) ? 1.0f : 0.0f;
    }

    // run filter
    for (i=0; i<n; i++)
        iirfilt_rrrf_execute(f, x[i], &y[i]);

    // destroy filter object
    iirfilt_rrrf_destroy(f);

    // run linear prediction algorithm
    liquid_lpc(y,n,p,a_hat,g_hat);

    // print results
    for (i=0; i<p; i++)
        printf("  a[%3u] = %12.8f, g[%3u] = %12.8f\n", i, a_hat[i], i, g_hat[i]);

    // 
    // plot results to output file
    //
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"\n");
    fprintf(fid,"p=%u;\n", p);
    fprintf(fid,"n=%u;\n",n);
    fprintf(fid,"x=zeros(1,n);\n");
    fprintf(fid,"y=zeros(1,n);\n");

    for (i=0; i<n; i++) {
        //printf("%4u : %12.8f + j*%12.8f\n", i, crealf(y), cimagf(y));
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));
        fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));
    }

    // plot output
    fprintf(fid,"t=0:(n-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"  plot(t,real(x),'-','Color',[1 1 1]*0.5,'LineWidth',1,...\n");
    fprintf(fid,"       t,real(y),'-','Color',[0 0.5 0.25],'LineWidth',2);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('real');\n");
    fprintf(fid,"  legend('input','filtered output',1);\n");
    fprintf(fid,"  grid on;\n");
    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

