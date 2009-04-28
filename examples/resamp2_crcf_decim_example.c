//
// Halfband decimator (complex)
//

#include <stdio.h>
#include <complex.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "resamp2_crcf_decim_example.m"

int main() {
    unsigned int m=5;
    unsigned int h_len = 4*m+1; // filter length
    float fc=0.17f;
    unsigned int N=128;

    resamp2_cccf f = resamp2_cccf_create(h_len);

    resamp2_cccf_print(f);

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"h_len=%u;\nN=%u;\n", h_len, N);

    unsigned int i;
    float theta=0.0f, dtheta=2*M_PI*fc;
    float complex x[2], y;
    for (i=0; i<N; i++) {
        x[0] = cexpf(_Complex_I*theta);
        theta += dtheta;
        x[1] = cexpf(_Complex_I*theta);
        theta += dtheta;

        resamp2_cccf_decim_execute(f, x, &y);

        fprintf(fid,"x(%3u) = %8.4f + j*%8.4f;\n", 2*i+1, crealf(x[0]), cimagf(x[0]));
        fprintf(fid,"x(%3u) = %8.4f + j*%8.4f;\n", 2*i+2, crealf(x[1]), cimagf(x[1]));
        fprintf(fid,"y(%3u) = %8.4f + j*%8.4f;\n", i+1,   crealf(y),    cimagf(y));

        printf("y(%3u) = %8.4f + j*%8.4f;\n", i+1, crealf(y), cimagf(y));
    }

    fprintf(fid,"nfft=512;\n");
    fprintf(fid,"X=20*log10(abs(fftshift(fft(x.*hamming(length(x))',nfft))));\n");
    fprintf(fid,"Y=20*log10(abs(fftshift(fft(y.*hamming(length(y))',nfft))));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure; plot(f,X,'Color',[0.5 0.5 0.5],f/2,Y,'LineWidth',2);\n");
    fprintf(fid,"grid on;\nxlabel('normalized frequency');\nylabel('PSD [dB]');\n");
    fprintf(fid,"legend('original','decimated',1);");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    resamp2_cccf_destroy(f);
    printf("done.\n");
    return 0;
}
