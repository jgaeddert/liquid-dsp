//
// Halfband interpolator (complex)
//

#include <stdio.h>
#include <complex.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "resamp_crcf_example.m"

int main() {
    unsigned int h_len = 13;    // filter semi-length (delay)
    float r=1.3f;               // resampling rate
    float slsl=-60.0f;          // sidelobe suppression level
    unsigned int npfb=16;       // number of filters in bank
    unsigned int n=128;         // number of input samples
    float fc=0.079f;            // sine carrier frequency

    resamp_crcf f = resamp_crcf_create(r,h_len,slsl,npfb);
    resamp_crcf_print(f);

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n",OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"h_len=%u;\n", h_len);
    fprintf(fid,"npfb=%u;\n",  npfb);
    fprintf(fid,"r=%12.8f;\n", r);

    unsigned int i, j, k=0;
    float theta=0.0f, dtheta=2*M_PI*fc;
    float complex x, y[20];
    unsigned int num_written;
    for (i=0; i<n; i++) {
        x = cexpf(_Complex_I*theta);
        theta += dtheta;

        resamp_crcf_execute(f, x, y, &num_written);

        printf("%3u : x = %12.8f + j*%12.8f,", i, crealf(x),cimagf(x));
        //for (j=0; j<num_written; j++)
        //    printf("");
        printf("\n");

        fprintf(fid,"x(%3u) = %8.4f + j*%8.4f;\n", i+1,   crealf(x),    cimagf(x));
        for (j=0; j<num_written; j++)
            fprintf(fid,"y(%3u) = %8.4f + j*%8.4f;\n", k+j+1, crealf(y[j]), cimagf(y[j]));
        k += num_written;
    }

    // plot results
    fprintf(fid,"%% plot frequency-domain result\n");
    fprintf(fid,"nfft=512;\n");
    fprintf(fid,"%% estimate PSD, normalize by hamming window gain, array length\n");
    fprintf(fid,"X=20*log10(abs(fftshift(fft(x.*hamming(length(x))',nfft)*1.85/length(x))));\n");
    fprintf(fid,"Y=20*log10(abs(fftshift(fft(y.*hamming(length(y))',nfft)*1.85/length(y))));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure; plot(f/r,X,'Color',[0.5 0.5 0.5],f,Y,'LineWidth',2);\n");
    fprintf(fid,"grid on;\nxlabel('normalized frequency');\nylabel('PSD [dB]');\n");
    fprintf(fid,"legend('original','resampled',1);");
    fprintf(fid,"axis([-0.5 0.5 -80 10]);\n");
    fprintf(fid,"\n\n");

    fprintf(fid,"%% plot time-domain result\n");
    fprintf(fid,"tx=[0:(length(x)-1)];\n");
    fprintf(fid,"ty=[0:(length(y)-1)]/r-h_len;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(tx,real(x),'-x','Color',[0.5 0.5 0.5],ty,real(y),'-x','Color',[0.5 0 0]);\n");
    fprintf(fid,"  legend('original','resampled',1);");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('real');\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(tx,imag(x),'-x','Color',[0.5 0.5 0.5],ty,imag(y),'-x','Color',[0 0.5 0]);\n");
    fprintf(fid,"  legend('original','resampled',1);");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('imag');\n");

    fclose(fid);
    printf("results written to %s\n",OUTPUT_FILENAME);

    resamp_crcf_destroy(f);
    printf("done.\n");
    return 0;
}
