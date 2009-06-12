//
//
//

#include <stdio.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "fir_filter_rrrf_example.m"

int main() {
    // options
    unsigned int h_len=17;  // filter length
    float fc=0.3f;          // filter cutoff
    float slsl=30.0f;       // sidelobe suppression level
    float mu=0.0f;          // timing offset
    unsigned int n=64;      // number of random input samples

    unsigned int i;
    float h[h_len];
    fir_kaiser_window(h_len,fc,slsl,mu,h);
    fir_filter_rrrf f = fir_filter_rrrf_create(h,h_len);
    //fir_filter_rrrf_print(f);

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% fir_filter_rrrf_example.m: auto-generated file\n\n");
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"h_len=%u;\nn=%u;\n", h_len, n);

    for (i=0; i<h_len; i++)
        fprintf(fid,"h(%4u) = %12.4e;\n", i+1, h[i]);

    float x, y;
    for (i=0; i<n+h_len; i++) {
        // generate noise
        x = (i<n) ? randnf()/sqrtf(n/2) : 0.0f;

        fir_filter_rrrf_push(f, x);
        fir_filter_rrrf_execute(f, &y); 

        fprintf(fid,"x(%4u) = %12.4e; y(%4u) = %12.4e;\n", i+1, x, i+1, y);
        printf("x(%4u) = %12.4e; y(%4u) = %12.4e;\n", i+1, x, i+1, y);
    }   

    fprintf(fid,"nfft=512;\n");
    fprintf(fid,"w=hamming(length(x))';\n");
    fprintf(fid,"H=20*log10(abs(fftshift(fft(h,   nfft))));\n");
    fprintf(fid,"X=20*log10(abs(fftshift(fft(x.*w,nfft))));\n");
    fprintf(fid,"Y=20*log10(abs(fftshift(fft(y.*w,nfft))));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure; plot(f,X,'Color',[0.3 0.3 0.3],f,Y,'LineWidth',2,f,H,'-b','LineWidth',2);\n");
    fprintf(fid,"grid on;\nxlabel('normalized frequency');\nylabel('PSD [dB]');\n");
    fprintf(fid,"legend('noise','filtered noise','filter prototype',1);");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    fir_filter_rrrf_destroy(f);

    printf("done.\n");
    return 0;
}

