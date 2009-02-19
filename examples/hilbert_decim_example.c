//
//
//

#include <stdio.h>
#include <complex.h>
#include <math.h>

#include "liquid.h"

#define DEBUG

int main() {
    unsigned int m=5;
    unsigned int h_len = 4*m+1; // filter length
    float fc=0.17f;
    unsigned int N=128;

    firhilb f = firhilb_create(h_len);

    firhilb_print(f);

#ifdef DEBUG
    FILE*fid = fopen("hilbert_decim_example.m","w");
    fprintf(fid,"%% hilbert example\nclear all;\nclose all;\n\n");
    fprintf(fid,"h_len=%u;\nN=%u;\n", h_len, N);
#endif

    unsigned int i;
    float x[2], theta=0.0f, dtheta=2*M_PI*fc;
    float complex y;
    for (i=0; i<N; i++) {
        x[0] = cosf(theta);
        theta += dtheta;
        x[1] = cosf(theta);
        theta += dtheta;

        firhilb_decim_execute(f, x, &y);


#ifdef DEBUG
        fprintf(fid,"x(%3u) = %8.4f;\n", 2*i+1, x[0]);
        fprintf(fid,"x(%3u) = %8.4f;\n", 2*i+2, x[1]);
        fprintf(fid,"y(%3u) = %8.4f + j*%8.4f;\n", i+1, crealf(y), cimagf(y));
#else
        printf("y(%3u) = %8.4f + j*%8.4f;\n", i+1, crealf(y), cimagf(y));
#endif
    }

#ifdef DEBUG
    fprintf(fid,"nfft=512;\n");
    fprintf(fid,"X=20*log10(abs(fftshift(fft(x.*hamming(length(x))',nfft))));\n");
    fprintf(fid,"Y=20*log10(abs(        (fft(y.*hamming(length(y))',nfft))));\n");
    fprintf(fid,"f =[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"fd=[0:(nfft-1)]/(2*nfft);\n");
    fprintf(fid,"figure; plot(f,X,'Color',[0.5 0.5 0.5],fd,Y,'LineWidth',2);\n");
    fprintf(fid,"grid on;\nxlabel('normalized frequency');\nylabel('PSD [dB]');\n");
    fprintf(fid,"legend('original/real','transformed/decimated',1);");

    fclose(fid);
    printf("results written to hilbert_decim_example.m\n");
#endif

    firhilb_destroy(f);
    printf("done.\n");
    return 0;
}
