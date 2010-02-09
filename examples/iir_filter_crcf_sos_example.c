//
// iir_filter_sos_crcf_example.c
//
// complex infinite impulse response filter example (second-order system)
//

#include <stdio.h>
#include <math.h>
#include <complex.h>

#include "liquid.internal.h"

#define OUTPUT_FILENAME "iir_filter_crcf_sos_example.m"

int main() {
    // from Mitra:2001, p. 625
    float B[9] = {
        0.0300018f,  0.0300018f, 0.0f,
        1.0000000f, -0.0861016f, 1.0000000f,
        1.0000000f, -0.7638292f, 1.0000000f
    };

    float A[9] = {
        1.0000000f, -0.6135686f, 0.0f,
        1.0000000f, -1.1528628f, 0.6116484f,
        1.0000000f, -1.0983340f, 0.89907598f
    };

    unsigned int nsos=3;
    unsigned int n=128;     // number of samples
    unsigned int i;

#if 0
    // design butterworth filter...
    float complex r[5];
    butter_rootsf(5,r);
    float complex z[5];
    float complex p[5];
    float fc = 0.1f;
    float m = 1.0 / tanf(3.14159*fc);
    float complex k = 1.0f;
    for (i=0; i<5; i++) {
        p[i] = (r[i]/m - 1.0f)/(r[i]/m + 1.0f);
        z[i] = 1.0f;
        k *= 1.0f/(r[i] + m);
    }
    iirdes_zpk2sos(z,p,5,crealf(k),B,A);
#endif

    printf("B:\n");
    for (i=0; i<nsos; i++)
        printf("  %12.8f %12.8f %12.8f\n", B[3*i+0], B[3*i+1], B[3*i+2]);

    printf("A:\n");
    for (i=0; i<nsos; i++)
        printf("  %12.8f %12.8f %12.8f\n", A[3*i+0], A[3*i+1], A[3*i+2]);

    //return 0;

    // create filter
    iir_filter_crcf f = iir_filter_crcf_create_sos(B,A,nsos);
    iir_filter_crcf_print(f);

    // open output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"\n");
    fprintf(fid,"nsos=%u;\n", nsos);
    fprintf(fid,"B=zeros(nsos,3);\n");
    fprintf(fid,"A=zeros(nsos,3);\n");
    fprintf(fid,"n=%u;\n",n);
    fprintf(fid,"x=zeros(1,n);\n");
    fprintf(fid,"y=zeros(1,n);\n");

    float complex x;
    float complex y;
    for (i=0; i<n; i++) {
        // generate input signal (noisy sine wave with decaying amplitude)
        x = cexpf((2*M_PI*0.057f*_Complex_I - 0.04f)*i);
        cawgn(&x,0.1f);

        // run filter
        iir_filter_crcf_execute(f, x, &y);

        //printf("%4u : %12.8f + j*%12.8f\n", i, crealf(y), cimagf(y));
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x), cimagf(x));
        fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y), cimagf(y));
    }

    // output filter coefficients using extra precision
    unsigned int j;
    for (i=0; i<nsos; i++) {
        for (j=0; j<3; j++) {
            fprintf(fid,"B(%3u,%3u) = %16.8e;\n", i+1, j+1, B[3*i+j]);
            fprintf(fid,"A(%3u,%3u) = %16.8e;\n", i+1, j+1, A[3*i+j]);
        }
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
    fprintf(fid,"\n");
    fprintf(fid,"nfft=1024;\n");
    fprintf(fid,"H = zeros(1,nfft);\n");
    fprintf(fid,"for i=1:nsos,\n");
    fprintf(fid,"    H = H + 20*log10(abs(fftshift(fft(B(i,:),nfft)./fft(A(i,:),nfft))));\n");
    fprintf(fid,"end;\n");
    fprintf(fid,"f = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,H,'-','Color',[0.5 0 0],'LineWidth',2);\n");
    fprintf(fid,"axis([-0.5 0.5 -60 10]);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('Normalized Frequency');\n");
    fprintf(fid,"ylabel('Filter PSD [dB]');\n");
    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    // destroy filter object
    iir_filter_crcf_destroy(f);

    printf("done.\n");
    return 0;
}

