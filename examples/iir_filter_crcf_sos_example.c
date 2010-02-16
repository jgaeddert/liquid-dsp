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
    // options
    unsigned int order=5;   // filter order
    float fc=0.2f;          // cutoff frequency
    unsigned int n=128;     // number of samples

    unsigned int i;
    unsigned int r = order % 2;
    unsigned int L = (order-r)/2;

    // design Chebyshev type I filter
    float complex za[order];    // analog complex zeros
    float complex pa[order];    // analog complex poles
    float complex ka;           // analog gain

    unsigned int nza = 0;
    unsigned int npa = order;

    nza = 2*L;
    float slsl = 40.0f;
    float epsilon = powf(10.0f, -slsl/20.0f);
    cheby2_azpkf(order,fc,epsilon,za,pa,&ka);

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
    float B[3*(L+r)];
    float A[3*(L+r)];
    iirdes_dzpk2sosf(zd,pd,order,kd,B,A);

    printf("B:\n");
    for (i=0; i<L+r; i++)
        printf("  %12.8f %12.8f %12.8f\n", B[3*i+0], B[3*i+1], B[3*i+2]);

    printf("A:\n");
    for (i=0; i<L+r; i++)
        printf("  %12.8f %12.8f %12.8f\n", A[3*i+0], A[3*i+1], A[3*i+2]);

    //return 0;

    // create filter
    iir_filter_crcf f = iir_filter_crcf_create_sos(B,A,L+r);
    iir_filter_crcf_print(f);

    // open output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"\n");
    fprintf(fid,"order=%u;\n", order);
    fprintf(fid,"nza=%u;\n", nza);
    fprintf(fid,"npa=%u;\n", npa);
    fprintf(fid,"nsos=%u;\n", L+r);
    fprintf(fid,"B=zeros(nsos,3);\n");
    fprintf(fid,"A=zeros(nsos,3);\n");
    fprintf(fid,"n=%u;\n",n);
    fprintf(fid,"x=zeros(1,n);\n");
    fprintf(fid,"y=zeros(1,n);\n");

#if 0
    // print analog z/p/k
    fprintf(fid,"za = zeros(1,nza);\n");
    for (i=0; i<nza; i++)
        fprintf(fid,"  za(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(za[i]), cimagf(za[i]));
    fprintf(fid,"pa = zeros(1,npa);\n");
    for (i=0; i<nza; i++)
        fprintf(fid,"  pa(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(pa[i]), cimagf(pa[i]));
#endif

    // print digital z/p/k
    fprintf(fid,"zd = zeros(1,order);\n");
    fprintf(fid,"pd = zeros(1,order);\n");
    for (i=0; i<order; i++) {
        fprintf(fid,"  zd(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(zd[i]), cimagf(zd[i]));
        fprintf(fid,"  pd(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(pd[i]), cimagf(pd[i]));
    }

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
    for (i=0; i<L+r; i++) {
        for (j=0; j<3; j++) {
            fprintf(fid,"B(%3u,%3u) = %16.8e;\n", i+1, j+1, B[3*i+j]);
            fprintf(fid,"A(%3u,%3u) = %16.8e;\n", i+1, j+1, A[3*i+j]);
        }
    }
    fprintf(fid,"\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"k=0:0.01:1;\n");
    fprintf(fid,"ti = cos(2*pi*k);\n");
    fprintf(fid,"tq = sin(2*pi*k);\n");
    fprintf(fid,"plot(ti,tq,'-','LineWidth',1,'Color',[1 1 1]*0.7,...\n");
    fprintf(fid,"     zd,'o','LineWidth',2,'Color',[0.5 0   0],'MarkerSize',2,...\n");
    fprintf(fid,"     pd,'x','LineWidth',2,'Color',[0   0.5 0],'MarkerSize',2);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"axis([-1 1 -1 1]*1.2);\n");
    fprintf(fid,"axis square;\n");

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

