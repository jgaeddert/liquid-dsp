//
//
//

#include <stdio.h>
#include <complex.h>
#include <math.h>

#include "../src/filter.h"

#define DEBUG

int main() {
    unsigned int m=3;
    unsigned int h_len = 4*m+1; // filter length
    float fc=0.13f;
    unsigned int N=64;

    firhilb f = firhilb_create(h_len);

#ifdef DEBUG
    FILE*fid = fopen("hilbert_example.m","w");
    fprintf(fid,"%% hilbert example\nclear all;\nclose all;\n\n");
    fprintf(fid,"h_len=%u;\nN=%u;\n", h_len, N);
#endif

    unsigned int i, n=0;
    float x[2], theta=0.0f, dtheta=2*M_PI*fc;
    float complex y;
    for (i=0; i<N; i+=2) {
        x[0] = cosf(theta);
        theta += dtheta;
        x[1] = cosf(theta);
        theta += dtheta;

        firhilb_execute(f, x, &y);

        printf("y(%3u) = %8.4f + j*%8.4f;\n", n+1, crealf(y), cimagf(y));

#ifdef DEBUG
        fprintf(fid,"x(%3u) = %8.4f;\n", 2*n+1, x[0]);
        fprintf(fid,"x(%3u) = %8.4f;\n", 2*n+2, x[1]);
        fprintf(fid,"y(%3u) = %8.4f + j*%8.4f;\n", n+1, crealf(y), cimagf(y));
#endif

        n++;
    }

#ifdef DEBUG
    fprintf(fid,"nfft=512;\n");
    fprintf(fid,"X=20*log10(abs(fftshift(fft(x.*hamming(length(x))',nfft))));\n");
    fprintf(fid,"Y=20*log10(abs(fftshift(fft(y.*hamming(length(y))',nfft))));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure; plot(f,X,'Color',[0.7 0.7 0.7],f,Y);\n");

    fclose(fid);
#endif

    firhilb_destroy(f);
    printf("done.\n");
    return 0;
}
