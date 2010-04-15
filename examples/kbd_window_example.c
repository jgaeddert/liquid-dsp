//
//
//

#include <stdio.h>

#include "liquid.h"

#define OUTPUT_FILENAME "kbd_window_example.m"

int main() {
    // options
    unsigned int n=20;      // window length
    float beta = 10.0f;     // Kaiser beta factor

    float w[n];
    unsigned int i;
    for (i=0; i<n; i++)
        w[i] = liquid_kbd_window(i,n,beta);

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"n=%u;\n",n);

    for (i=0; i<n; i++) {
        fprintf(fid,"w(%4u) = %12.4e;\n", i+1, w[i]);
    }   

    fprintf(fid,"nfft=1024;\n");
    fprintf(fid,"W=20*log10(abs(fftshift(fft(w/sum(w),nfft))));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure; plot(f,W,'Color',[0 0.5 0.25],'LineWidth',2);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('normalized frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"title(['Kaiser-Bessel derived window']);\n");
    fprintf(fid,"axis([-0.5 0.5 -120 20]);\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

