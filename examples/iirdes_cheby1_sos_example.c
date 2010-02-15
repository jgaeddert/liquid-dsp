//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "iirdes_cheby1_sos_example.m"

int main() {
    // options
    unsigned int n=12;      // filter order
    float fc = 0.25f;       // cutoff
    float ripple = 1.0f;    // passband ripple [dB]

    // epsilon
    float epsilon = sqrtf( powf(10.0f, ripple / 10.0f) - 1.0f );

    unsigned int r = n%2;
    unsigned int L = (n-r)/2;
    float B[3*(L+r)];       // numerator
    float A[3*(L+r)];       // denominator

    cheby1sosf(n,fc,epsilon,B,A);

    // open output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"\nclear all;\nclose all;\n\n");
    fprintf(fid,"n=%u;\n", n);
    fprintf(fid,"r=%u;\n", r);
    fprintf(fid,"L=%u;\n", L);
    fprintf(fid,"B = zeros(L+r,3);\n");
    fprintf(fid,"A = zeros(L+r,3);\n");
    // output filter coefficients using extra precision
    unsigned int i;
    unsigned int j;
    for (i=0; i<L; i++) {
        for (j=0; j<3; j++) {
            fprintf(fid,"B(%3u,%3u) = %16.8e;\n", i+1, j+1, B[3*i+j]);
            fprintf(fid,"A(%3u,%3u) = %16.8e;\n", i+1, j+1, A[3*i+j]);
        }
    }
    fprintf(fid,"\n");
    fprintf(fid,"nfft=1024;\n");
    fprintf(fid,"H = zeros(1,nfft);\n");
    fprintf(fid,"for i=1:L,\n");
    fprintf(fid,"    H = H + 20*log10(abs(fftshift(fft(B(i,:),nfft)./fft(A(i,:),nfft))));\n");
    fprintf(fid,"end;\n");
    fprintf(fid,"f = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,H,'-','Color',[0.5 0 0],'LineWidth',2);\n");
    fprintf(fid,"axis([0.0 0.5 -4 1]);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('Normalized Frequency');\n");
    fprintf(fid,"ylabel('Filter PSD [dB]');\n");
    fclose(fid);
    printf("results written to %s.\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

