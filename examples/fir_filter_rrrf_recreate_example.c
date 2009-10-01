//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "liquid.h"

#define OUTPUT_FILENAME "fir_filter_rrrf_recreate_example.m"

int main() {
    srand(time(NULL));
    // options
    unsigned int k=4;
    unsigned int m0=2;
    unsigned int m1=6;
    float beta0 = 0.3f;
    float beta1 = 0.3f;
    unsigned int n=256;

    unsigned int i;
    unsigned int h0_len = 2*k*m0 + 1;
    unsigned int h1_len = 2*k*m1 + 1;
    float h0[h0_len];
    float h1[h1_len];
    design_rcos_filter(k,m0,beta0,0,h0);
    design_rcos_filter(k,m1,beta1,0,h1);
    fir_filter_rrrf f = fir_filter_rrrf_create(h0,h0_len);
    //fir_filter_rrrf_print(f);

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% fir_filter_rrrf_example.m: auto-generated file\n\n");
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"h0_len=%u;\n", h0_len);
    fprintf(fid,"h1_len=%u;\n", h1_len);

    for (i=0; i<h0_len; i++)
        fprintf(fid,"h0(%4u) = %12.4e;\n", i+1, h0[i]);
    for (i=0; i<h1_len; i++)
        fprintf(fid,"h1(%4u) = %12.4e;\n", i+1, h1[i]);

    float x, y;
    unsigned int ix=0, iy=0;
    for (i=0; i<n/2; i++) {
        // generate random BPSK signal
        if ((ix%k)==0)  x = rand()%2 ? 1.0f : -1.0f;
        else            x = 0.0f;

        fir_filter_rrrf_push(f, x);
        fir_filter_rrrf_execute(f, &y); 

        ix++;
        iy++;
        fprintf(fid,"x(%4u) = %12.4e; y(%4u) = %12.4e;\n", ix+1, x, iy+1, y);
        printf("x(%4u) = %12.4e; y(%4u) = %12.4e;\n", ix+1, x, iy+1, y);
    }

    // recreate filter
    printf("re-creating filter...\n");
    f = fir_filter_rrrf_recreate(f,h1,h1_len);

    // push additional inputs to compensate for increased filter delay
    if (h1_len > h0_len) {
        unsigned int t = (h1_len - h0_len)/2;
        for (i=0; i<t; i++) {
            if ((ix%k)==0)  x = rand()%2 ? 1.0f : -1.0f;
            else            x = 0.0f;
            ix++;
            fir_filter_rrrf_push(f, x);
            fprintf(fid,"x(%4u) = %12.4e;\n", ix+1, x);
            printf("x(%4u) = %12.4e;\n", ix+1, x);
        }
    }

    for (i=0; i<n/2; i++) {
        // generate random BPSK signal
        if ((ix%k)==0)  x = rand()%2 ? 1.0f : -1.0f;
        else            x = 0.0f;

        fir_filter_rrrf_push(f, x);
        fir_filter_rrrf_execute(f, &y); 

        ix++;
        iy++;
        fprintf(fid,"x(%4u) = %12.4e; y(%4u) = %12.4e;\n", ix+1, x, iy+1, y);
        printf("x(%4u) = %12.4e; y(%4u) = %12.4e;\n", ix+1, x, iy+1, y);
    }

    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(1:length(x),x,[1:length(y)]-[h0_len-1]/2,y);\n");
    fprintf(fid,"xlabel('time');\n");
    fprintf(fid,"ylabel('signal');\n");
    fprintf(fid,"legend('symbol','interp',1);\n");

    fprintf(fid,"\n\n");
    fprintf(fid,"n  = round(length(y)/2);\n");
    fprintf(fid,"w  = hamming(n).';\n");
    //fprintf(fid,"w  = kaiser(n,10);\n");
    fprintf(fid,"y0 = y(1:n);\n");
    fprintf(fid,"y1 = y((length(y)-n+1):end);\n");
    fprintf(fid,"nfft = 1024;\n");
    fprintf(fid,"f = [0:nfft-1]/nfft - 0.5;\n");
    fprintf(fid,"Y0 = 20*log10(abs(fftshift(fft(y0.*w,nfft))));\n");
    fprintf(fid,"Y1 = 20*log10(abs(fftshift(fft(y1.*w,nfft))));\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,Y0,'-',f,Y1,'-');\n");
    fprintf(fid,"xlabel('Normalized Frequency');\n");
    fprintf(fid,"ylabel('Power spectral density [dB]');\n");
    fprintf(fid,"legend('filter 0','filter 1',1);\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    fir_filter_rrrf_destroy(f);

    printf("done.\n");
    return 0;
}

