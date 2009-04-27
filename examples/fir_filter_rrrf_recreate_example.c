//
//
//

#include <stdio.h>

#include "liquid.h"

#define DEBUG_FILENAME "fir_filter_rrrf_recreate_example.m"

int main() {
    // options
    unsigned int k=4;
    unsigned int m0=1;
    unsigned int m1=4;
    float beta=0.33f;
    unsigned int n=256;

    unsigned int i;
    unsigned int h0_len = 2*k*m0 + 1;
    unsigned int h1_len = 2*k*m1 + 1;
    float h0[h0_len];
    float h1[h1_len];
    /*
    fir_kaiser_window(h0_len,0.7f,20.0f,h0);
    fir_kaiser_window(h1_len,0.1f,60.0f,h1);
    */
    design_rcos_filter(k,m0,beta,0,h0);
    design_rcos_filter(k,m1,beta,0,h1);
    fir_filter_rrrf f = fir_filter_rrrf_create(h0,h0_len);
    //fir_filter_rrrf_print(f);

    FILE*fid = fopen(DEBUG_FILENAME,"w");
    fprintf(fid,"%% fir_filter_rrrf_example.m: auto-generated file\n\n");
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"h0_len=%u;\nn=%u;\n", h0_len, n);
    fprintf(fid,"h1_len=%u;\nn=%u;\n", h1_len, n);

    for (i=0; i<h0_len; i++)
        fprintf(fid,"h0(%4u) = %12.4e;\n", i+1, h0[i]);
    for (i=0; i<h1_len; i++)
        fprintf(fid,"h1(%4u) = %12.4e;\n", i+1, h1[i]);

    float x, y;
    for (i=0; i<n/2; i++) {
        // generate noise
        x = randnf();

        fir_filter_rrrf_push(f, x);
        fir_filter_rrrf_execute(f, &y); 

        fprintf(fid,"x(%4u) = %12.4e; y(%4u) = %12.4e;\n", i+1, x, i+1, y);
        printf("x(%4u) = %12.4e; y(%4u) = %12.4e;\n", i+1, x, i+1, y);
    }

    // recreate filter
    printf("re-creating filter...\n");
    f = fir_filter_rrrf_recreate(f,h1,h1_len);

    for (; i<n; i++) {
        // generate noise
        x = randnf();

        fir_filter_rrrf_push(f, x);
        fir_filter_rrrf_execute(f, &y); 

        fprintf(fid,"x(%4u) = %12.4e; y(%4u) = %12.4e;\n", i+1, x, i+1, y);
        printf("x(%4u) = %12.4e; y(%4u) = %12.4e;\n", i+1, x, i+1, y);
    }

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
    printf("results written to %s\n", DEBUG_FILENAME);

    fir_filter_rrrf_destroy(f);

    printf("done.\n");
    return 0;
}

