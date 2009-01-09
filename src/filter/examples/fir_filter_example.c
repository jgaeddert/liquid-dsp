//
//
//

#include <stdio.h>

#include "../src/filter.h"
#include "../src/firdes.h"
#include "../../random/src/random.h"

#define DEBUG 1
#define DEBUG_FILENAME "fir_filter_example.m"

int main() {
    // options
    unsigned int h_len=31;
    float fc=0.3f;
    float slsl=30.0f;
    unsigned int n=64;

    unsigned int i;
    float h[h_len];
    fir_kaiser_window(h_len,fc,slsl,h);
    fir_filter f = fir_filter_create(h,h_len);
    //fir_filter_print(f);

#if DEBUG
    FILE*fid = fopen(DEBUG_FILENAME,"w");
    fprintf(fid,"%% fir_filter_example.m: auto-generated file\n\n");
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"h_len=%u;\nn=%u;\n", h_len, n);

    for (i=0; i<h_len; i++)
        fprintf(fid,"h(%4u) = %12.4e;\n", i+1, h[i]);
#endif

    float x, y;
    for (i=0; i<n; i++) {
        // generate noise
        x = randnf();

        fir_filter_push(f, x);
        fir_filter_execute(f, &y); 

#if DEBUG
        fprintf(fid,"x(%4u) = %12.4e; y(%4u) = %12.4e;\n", i+1, x, i+1, y);
#else
        printf("x(%4u) = %12.4e; y(%4u) = %12.4e;\n", i+1, x, i+1, y);
#endif
    }   

#if DEBUG
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
    printf("results written to %s\n", DEBUG_FILENAME);
#endif


    fir_filter_destroy(f);

    printf("done.\n");
    return 0;
}

