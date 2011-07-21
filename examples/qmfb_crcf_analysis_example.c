//
// quadrature mirror filterbank analyzer example
//
// Uses a quadrture mirror filterbank to decompose a
// signal into its upper and lower frequency band
// components.
//

#include <stdio.h>
#include <complex.h>
#include <math.h>

#include "liquid.experimental.h"

#define OUTPUT_FILENAME "qmfb_crcf_analysis_example.m"

int main() {
    unsigned int h_len=20;              // filter length
    float beta = 0.3f;                  // stop-band attenuation
    unsigned int num_samples=64;        // number of samples

    // derived values
    unsigned int n = num_samples + h_len;   // extend length of analysis to
                                        // incorporate filter delay

    // create filterbank
    qmfb_crcf q = qmfb_crcf_create(h_len, beta, LIQUID_QMFB_ANALYZER);
    qmfb_crcf_print(q);

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"n=%u;\n", n);
    fprintf(fid,"x = zeros(1,%u);\n", 2*n);
    fprintf(fid,"y = zeros(2,%u);\n",   n);

    unsigned int i;
    float complex x[2*n], y[2][n];

    // generate time-domain signal (windowed sinusoidal pulses)
    nco_crcf nco_0 = nco_crcf_create(LIQUID_VCO);
    nco_crcf nco_1 = nco_crcf_create(LIQUID_VCO);
    nco_crcf_set_frequency(nco_0, 0.122*M_PI);
    nco_crcf_set_frequency(nco_1, 0.779*M_PI);
    float complex x0,x1;
    for (i=0; i<2*num_samples; i++) {
        nco_crcf_cexpf(nco_0, &x0);
        nco_crcf_cexpf(nco_1, &x1);
        x[i] = (x0 + x1) * kaiser(i,2*num_samples,10.0f,0.0f);
        nco_crcf_step(nco_0);
        nco_crcf_step(nco_1);
    }
    // pad end with zeros
    for (i=2*num_samples; i<2*n; i++)
        x[i] = 0.0f;

    // compute QMF sub-channel output
    for (i=0; i<n; i++) {
        qmfb_crcf_execute(q, x[2*i+0], x[2*i+1], y[0]+i, y[1]+i);
    }

    // write results to output file
    for (i=0; i<n; i++) {
        fprintf(fid,"x(%3u) = %8.4f + j*%8.4f;\n", 2*i+1, crealf(x[2*i+0]), cimagf(x[2*i+0]));
        fprintf(fid,"x(%3u) = %8.4f + j*%8.4f;\n", 2*i+2, crealf(x[2*i+1]), cimagf(x[2*i+1]));

        fprintf(fid,"y(1,%3u) = %8.4f + j*%8.4f;\n", i+1, crealf(y[0][i]),  cimagf(y[0][i]));
        fprintf(fid,"y(2,%3u) = %8.4f + j*%8.4f;\n", i+1, crealf(y[1][i]),  cimagf(y[1][i]));
    }

    // plot time-domain results
    fprintf(fid,"t0=0:(2*n-1);\n");
    fprintf(fid,"t1=0:(n-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(3,1,1); plot(t0,real(x),t0,imag(x)); ylabel('x(t)');\n");
    fprintf(fid,"subplot(3,1,2); plot(t1,real(y(1,:)),t1,imag(y(1,:))); ylabel('y_0(t)');\n");
    fprintf(fid,"subplot(3,1,3); plot(t1,real(y(2,:)),t1,imag(y(2,:))); ylabel('y_1(t)');\n");

    // plot freq-domain results
    fprintf(fid,"nfft=512; %% must be even number\n");
    fprintf(fid,"X=20*log10(abs(fftshift(fft(x/n,nfft))));\n");
    fprintf(fid,"Y0=20*log10(abs(fftshift(fft(y(1,:)/n,nfft))));\n");
    fprintf(fid,"Y1=20*log10(abs(fftshift(fft(y(2,:)/n,nfft))));\n");
    // Y1 needs to be split into two regions
    fprintf(fid,"f=[0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"t0 = [1]:[nfft/2];\n");
    fprintf(fid,"t1 = [nfft/2+1]:[nfft];\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"hold on;\n");
    fprintf(fid,"    plot(f,X,'Color',[0.5 0.5 0.5]);\n");
    fprintf(fid,"    plot(f/2,Y0,'LineWidth',2,'Color',[0 0.5 0]);\n");
    fprintf(fid,"    plot(f(t0)/2+0.5,Y1(t0),'LineWidth',2,'Color',[0.5 0 0]);\n");
    fprintf(fid,"    plot(f(t1)/2-0.5,Y1(t1),'LineWidth',2,'Color',[0.5 0 0]);\n");
    fprintf(fid,"hold off;\n");
    fprintf(fid,"grid on;\nxlabel('normalized frequency');\nylabel('PSD [dB]');\n");
    fprintf(fid,"legend('original','Y_0','Y_1',1);");
    fprintf(fid,"axis([-0.5 0.5 -140 20]);\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    qmfb_crcf_destroy(q);
    nco_crcf_destroy(nco_0);
    nco_crcf_destroy(nco_1);
    printf("done.\n");
    return 0;
}
