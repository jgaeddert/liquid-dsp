//
// iirfilt_crcf_dcblocker_example.c
//
// This example demonstrates how to create a DC-blocking recursive
// (infinite impulse response) filter.
//

#include <stdio.h>
#include <math.h>
#include <complex.h>

#include "liquid.h"

#define OUTPUT_FILENAME "iirfilt_crcf_dcblocker_example.m"

int main() {
    // options
    unsigned int num_samples = 1200;    // number of samples
    float        alpha       = 0.10f;   // filter cut-off

    // design filter from prototype
    iirfilt_crcf q = iirfilt_crcf_create_dc_blocker(alpha);
    iirfilt_crcf_print(q);

    // allocate memory for data arrays
    float complex x[num_samples];   // input
    float complex y[num_samples];   // output

    // generate input signal (sine wave with slowly-varying DC offset)
    unsigned int i;
    for (i=0; i<num_samples; i++) {
        x[i] =       cexpf( 0.400f*_Complex_I*i );
        x[i] += 2.0f*cexpf( 0.009f*_Complex_I*i );

        // run filter
        iirfilt_crcf_execute(q, x[i], &y[i]);
    }

    // destroy filter object
    iirfilt_crcf_destroy(q);

    // 
    // plot results to output file
    //
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"\n");
    fprintf(fid,"num_samples=%u;\n",num_samples);
    fprintf(fid,"x=zeros(1,num_samples);\n");
    fprintf(fid,"y=zeros(1,num_samples);\n");

    // save input, output arrays
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));
        fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));
    }

    // plot output
    fprintf(fid,"t=0:(num_samples-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(t,real(x),'-','Color',[1 1 1]*0.5,'LineWidth',1,...\n");
    fprintf(fid,"       t,real(y),'-','Color',[0 0.5 0.25],'LineWidth',2);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('real');\n");
    fprintf(fid,"  legend('input','filtered output','location','northeast');\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(t,imag(x),'-','Color',[1 1 1]*0.5,'LineWidth',1,...\n");
    fprintf(fid,"       t,imag(y),'-','Color',[0 0.25 0.5],'LineWidth',2);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('imag');\n");
    fprintf(fid,"  legend('input','filtered output','location','northeast');\n");
    fprintf(fid,"  grid on;\n");

    printf("done.\n");
    return 0;
}

