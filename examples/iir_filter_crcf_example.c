//
// iir_filter_crcf_example.c
//
// complex infinite impulse response filter example
//
// Demonstrates the functionality of iir_filter by designing a low-order
// prototype (e.g. Butterworth) and using it to filter a noisy signal.
// The filter coefficients are real, but the input and output arrays are
// complex.  The filter order and cutoff frequency are specified at the
// beginning.
//

#include <stdio.h>
#include <math.h>
#include <complex.h>

#include "liquid.h"

#define OUTPUT_FILENAME "iir_filter_crcf_example.m"

int main() {
    // options
    unsigned int order=4;   // filter order
    float fc=0.1f;          // cutoff frequency
    unsigned int n=128;     // number of samples

    // design butterworth filter
    float complex za[order];    // analog complex zeros
    float complex pa[order];    // analog complex poles
    float complex ka;           // analog gain
    butter_azpkf(order,fc,za,pa,&ka);
    unsigned int nza = 0;
    unsigned int npa = order;

    // complex digital poles/zeros/gain
    float complex zd[order];
    float complex pd[order];
    float complex kd;
    float m = 1.0f / tanf(M_PI * fc);
    iirdes_zpka2df(za,    nza,
                   pa,    npa,
                   ka,    m,
                   zd, pd, &kd);

    // convert complex digital poles/zeros/gain into transfer function
    float a[order+1];
    float b[order+1];
    iirdes_dzpk2tff(zd,pd,order,kd,b,a);

    // create filter
    iir_filter_crcf f = iir_filter_crcf_create(b,order+1,a,order+1);
    iir_filter_crcf_print(f);

    // open output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"\n");
    fprintf(fid,"order=%u;\n", order);
    fprintf(fid,"b=zeros(1,order+1);\n");
    fprintf(fid,"a=zeros(1,order+1);\n");
    fprintf(fid,"n=%u;\n",n);
    fprintf(fid,"x=zeros(1,n);\n");
    fprintf(fid,"y=zeros(1,n);\n");

    unsigned int i;
    float complex x;
    float complex y;
    for (i=0; i<n; i++) {
        // generate input signal (noisy sine wave with decaying amplitude)
        x = cexpf((2*M_PI*0.057f*_Complex_I - 0.04f)*i);
        cawgn(&x,0.1f);

        // run filter
        iir_filter_crcf_execute(f, x, &y);

        printf("%4u : %12.8f + j*%12.8f\n", i, crealf(y), cimagf(y));
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x), cimagf(x));
        fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y), cimagf(y));
    }

    // output filter coefficients using extra precision
    for (i=0; i<=order; i++) {
        fprintf(fid,"b(%3u) = %16.8e;\n", i+1, b[i]);
        fprintf(fid,"a(%3u) = %16.8e;\n", i+1, a[i]);
    }
    fprintf(fid,"\n");
    fprintf(fid,"t=0:(n-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(t,real(x),'-','Color',[1 1 1]*0.5,'LineWidth',1,...\n");
    fprintf(fid,"       t,real(y),'-','Color',[0 0.5 0.25],'LineWidth',2);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('real');\n");
    fprintf(fid,"  legend('input','filtered output',1);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(t,imag(x),'-','Color',[1 1 1]*0.5,'LineWidth',1,...\n");
    fprintf(fid,"       t,imag(y),'-','Color',[0 0.25 0.5],'LineWidth',2);\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('imag');\n");
    fprintf(fid,"  legend('input','filtered output',1);\n");
    fprintf(fid,"  grid on;\n");
    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    // destroy filter object
    iir_filter_crcf_destroy(f);

    printf("done.\n");
    return 0;
}

