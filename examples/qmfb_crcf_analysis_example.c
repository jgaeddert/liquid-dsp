//
// quadrature mirror filterbank analyzer
//

#include <stdio.h>
#include <complex.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "qmfb_crcf_analysis_example.m"

int main() {
    unsigned int m=5;   // filter delay
    unsigned int n=64;  // number of samples

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
    float theta=0.0f;
    float d_theta = 0.22f*(2*M_PI);

    for (i=0; i<n; i++) {
        x[2*i+0] = cexpf(_Complex_I*theta);
        theta += d_theta;
        x[2*i+1] = cexpf(_Complex_I*theta);
        theta += d_theta;
    }

    for (i=0; i<n; i++) {
        qmfb_crcf_analysis_execute(f, x[2*i+0], x[2*i+1], y[0]+i, y[1]+i);
    }

    for (i=0; i<n; i++) {
        fprintf(fid,"x(%3u) = %8.4f + j*%8.4f;\n", 2*i+1, crealf(x[2*i+0]), cimagf(x[2*i+0]));
        fprintf(fid,"x(%3u) = %8.4f + j*%8.4f;\n", 2*i+2, crealf(x[2*i+1]), cimagf(x[2*i+1]));

        fprintf(fid,"y(1,%3u) = %8.4f + j*%8.4f;\n", i+1, crealf(y[0][i]), cimagf(y[0][i]));
        fprintf(fid,"y(2,%3u) = %8.4f + j*%8.4f;\n", i+1, crealf(y[1][i]), cimagf(y[1][i]));

        //printf("y(%3u) = %8.4f + j*%8.4f;\n", i+1, crealf(y), cimagf(y));
    }

    fprintf(fid,"nfft=512;\n");
#if 0
    fprintf(fid,"X=20*log10(abs((fft(x.*hamming(2*n)',nfft))));\n");
    fprintf(fid,"Y0=20*log10(abs((fft(y(1,:).*hamming(n)',nfft))));\n");
    fprintf(fid,"Y1=20*log10(abs((fft(y(2,:).*hamming(n)',nfft))));\n");
#else
    fprintf(fid,"X=20*log10(abs((fft(x,nfft))));\n");
    fprintf(fid,"Y0=20*log10(abs((fft(y(1,:),nfft))));\n");
    fprintf(fid,"Y1=20*log10(abs((fft(y(2,:),nfft))));\n");
#endif
    fprintf(fid,"f=[0:(nfft-1)]/nfft;\n");
    fprintf(fid,"figure; plot(f,X,'Color',[0.5 0.5 0.5],f/2,Y0,'LineWidth',2,0.5+f/2,Y1,'LineWidth',2);\n");
    fprintf(fid,"grid on;\nxlabel('normalized frequency');\nylabel('PSD [dB]');\n");
    fprintf(fid,"legend('original','decimated',1);");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    qmfb_crcf_destroy(f);
    printf("done.\n");
    return 0;
}
