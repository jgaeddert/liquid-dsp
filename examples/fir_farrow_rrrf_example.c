//
//
//

#include <stdio.h>

#include "liquid.h"

#define OUTPUT_FILENAME "fir_farrow_rrrf_example.m"

int main() {
    // options
    unsigned int h_len=17;  // filter length
    unsigned int p=3;
    float fc=0.9f;          // filter cutoff
    float slsl=60.0f;       // sidelobe suppression level
    //float mu=0.0f;          // timing offset
    unsigned int nfft=64;      // 

    // coefficients array
    float h[h_len];

    fir_farrow_rrrf f = fir_farrow_rrrf_create(h_len, p, slsl);

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% fir_filter_rrrf_example.m: auto-generated file\n\n");
    fprintf(fid,"clear all;\nclose all;\n\n");
    //fprintf(fid,"h_len=%u;\nn=%u;\n", h_len, n);

    unsigned int i=0, j;
    fir_farrow_rrrf_set_delay(f,0.0f);
    fir_farrow_rrrf_get_coefficients(f,h);
    for (j=0; j<h_len; j++)
        fprintf(fid,"  h(%3u,%3u) = %12.4e;\n", i+1, j+1, h[j]);

   
#if 0
    fprintf(fid,"nfft=512;\n");
    fprintf(fid,"w=hamming(length(x))';\n");
    fprintf(fid,"H=20*log10(abs(fftshift(fft(h,   nfft))));\n");
    fprintf(fid,"X=20*log10(abs(fftshift(fft(x.*w,nfft))));\n");
    fprintf(fid,"Y=20*log10(abs(fftshift(fft(y.*w,nfft))));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure; plot(f,X,'Color',[0.3 0.3 0.3],f,Y,'LineWidth',2,f,H,'-b','LineWidth',2);\n");
    fprintf(fid,"grid on;\nxlabel('normalized frequency');\nylabel('PSD [dB]');\n");
    fprintf(fid,"legend('noise','filtered noise','filter prototype',1);");
#endif

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    fir_farrow_rrrf_destroy(f);

    printf("done.\n");
    return 0;
}

