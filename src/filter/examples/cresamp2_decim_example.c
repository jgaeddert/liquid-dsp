//
// Halfband decimator (complex)
//

#include <stdio.h>
#include <complex.h>
#include <math.h>

#include "../src/filter.h"

#define DEBUG

int main() {
    unsigned int m=5;
    unsigned int h_len = 4*m+1; // filter length
    float fc=0.17f;
    unsigned int N=128;

    cresamp2 f = cresamp2_create(h_len);

    cresamp2_print(f);

#ifdef DEBUG
    FILE*fid = fopen("cresamp2_decim_example.m","w");
    fprintf(fid,"%% hilbert example\nclear all;\nclose all;\n\n");
    fprintf(fid,"h_len=%u;\nN=%u;\n", h_len, N);
#endif

    unsigned int i;
    float theta=0.0f, dtheta=2*M_PI*fc;
    float complex x[2], y;
    for (i=0; i<N; i++) {
        x[0] = cexpf(_Complex_I*theta);
        theta += dtheta;
        x[1] = cexpf(_Complex_I*theta);
        theta += dtheta;

        cresamp2_decim_execute(f, x, &y);

#ifdef DEBUG
        fprintf(fid,"x(%3u) = %8.4f + j*%8.4f;\n", 2*i+1, crealf(x[0]), cimagf(x[0]));
        fprintf(fid,"x(%3u) = %8.4f + j*%8.4f;\n", 2*i+2, crealf(x[1]), cimagf(x[1]));
        fprintf(fid,"y(%3u) = %8.4f + j*%8.4f;\n", i+1,   crealf(y),    cimagf(y));
#else
        printf("y(%3u) = %8.4f + j*%8.4f;\n", i+1, crealf(y), cimagf(y));
#endif
    }

#ifdef DEBUG
    fprintf(fid,"nfft=512;\n");
    fprintf(fid,"X=20*log10(abs(fftshift(fft(x.*hamming(length(x))',nfft))));\n");
    fprintf(fid,"Y=20*log10(abs(fftshift(fft(y.*hamming(length(y))',nfft))));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure; plot(f,X,'Color',[0.5 0.5 0.5],f/2,Y,'LineWidth',2);\n");
    fprintf(fid,"grid on;\nxlabel('normalized frequency');\nylabel('PSD [dB]');\n");
    fprintf(fid,"legend('original','decimated',1);");

    fclose(fid);
    printf("results written to cresamp2_decim_example.m\n");
#endif

    cresamp2_destroy(f);
    printf("done.\n");
    return 0;
}
