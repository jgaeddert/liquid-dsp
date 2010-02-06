//
// iir_filter_rrrf_example.c
//

#include <stdio.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "iir_filter_rrrf_example.m"

int main() {
    // options
    unsigned int order=4;   // filter order
    float fc=0.1f;          // cutoff frequency
    unsigned int n=128;     // number of samples

    // design butterworth filter
    float a[order+1];
    float b[order+1];
    butterf(order,fc,b,a);
    //cheby1f(order,fc,0.1f,b,a);
    //cheby2f(order,fc,0.1f,b,a);

    iir_filter_rrrf f = iir_filter_rrrf_create(b,order+1,a,order+1);
    iir_filter_rrrf_print(f);

    // open output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"\n");
    fprintf(fid,"n=%u;\n",n);
    fprintf(fid,"x=zeros(1,n);\n");
    fprintf(fid,"y=zeros(1,n);\n");

    unsigned int i;
    float x;
    float y;
    for (i=0; i<n; i++) {
        // generate input signal (noisy sine wave with decaying amplitude)
        x = sinf(2*M_PI*0.057f*i)*expf(-(float)i*0.04f) + 0.1f*randnf();

        // run filter
        iir_filter_rrrf_execute(f, x, &y);

        printf("%4u : %12.8f\n", i, y);
        fprintf(fid,"x(%4u) = %12.4e;\n", i+1, x);
        fprintf(fid,"y(%4u) = %12.4e;\n", i+1, y);
    }

    fprintf(fid,"\n");
    fprintf(fid,"t=0:(n-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,x,'-','Color',[1 1 1]*0.5,'LineWidth',1,...\n");
    fprintf(fid,"     t,y,'-','Color',[0 0.5 0.25],'LineWidth',2);\n");
    fprintf(fid,"xlabel('time');\n");
    fprintf(fid,"ylabel('signal');\n");
    fprintf(fid,"legend('input','filtered output',1);\n");
    fprintf(fid,"grid on;\n");
    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    // destroy filter object
    iir_filter_rrrf_destroy(f);

    printf("done.\n");
    return 0;
}

