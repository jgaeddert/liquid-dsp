//
// quadrature mirror filterbank analyzer
//

#include <stdio.h>
#include <complex.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "qmfb_crcf_analysis_example.m"

int main() {
    unsigned int m=5;               // filter delay
    unsigned int num_samples=64;    // number of samples

    // derived values
    unsigned int n = num_samples + m;   // extend length of analysis to
                                        // incorporate filter delay

    // create filterbank
    qmfb_crcf f = qmfb_crcf_create(m);

    qmfb_crcf_print(f);

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"n=%u;\n", n);
    fprintf(fid,"x = zeros(1,%u);\n", 2*n);
    fprintf(fid,"y = zeros(2,%u);\n",   n);

    unsigned int i;
    float complex x[2*n], y[2][n];

    // generate time-domain signal (windowed sinusoidal pulse)
    float theta=0.0f;
    float d_theta = 0.122f*(2*M_PI);
    for (i=0; i<n; i++) {
        if (i<num_samples) {
            x[2*i+0] = cexpf(_Complex_I*theta) * kaiser(2*i+0,2*num_samples,10.0f);
            theta += d_theta;
            x[2*i+1] = cexpf(_Complex_I*theta) * kaiser(2*i+1,2*num_samples,10.0f);
            theta += d_theta;
        } else {
            x[2*i+0] = 0.0f;
            x[2*i+1] = 0.0f;
        }
    }

    // compute QMF sub-channel output
    for (i=0; i<n; i++) {
        qmfb_crcf_analysis_execute(f, x[2*i+0], x[2*i+1], y[0]+i, y[1]+i);
    }

    // output results
    for (i=0; i<n; i++) {
        fprintf(fid,"x(%3u) = %8.4f + j*%8.4f;\n", 2*i+1, crealf(x[2*i+0]), cimagf(x[2*i+0]));
        fprintf(fid,"x(%3u) = %8.4f + j*%8.4f;\n", 2*i+2, crealf(x[2*i+1]), cimagf(x[2*i+1]));

        fprintf(fid,"y(1,%3u) = %8.4f + j*%8.4f;\n", i+1, crealf(y[0][i]), cimagf(y[0][i]));
        fprintf(fid,"y(2,%3u) = %8.4f + j*%8.4f;\n", i+1, crealf(y[1][i]), cimagf(y[1][i]));

        //printf("y(%3u) = %8.4f + j*%8.4f;\n", i+1, crealf(y), cimagf(y));
    }

    // plot results
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

    qmfb_crcf_destroy(f);
    printf("done.\n");
    return 0;
}
