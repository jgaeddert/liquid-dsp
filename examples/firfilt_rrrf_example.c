//
// firfilt_rrrf_example.c
//

#include <stdio.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "firfilt_rrrf_example.m"

int main() {
    // options
    unsigned int h_len=17;  // filter length
    float fc=0.2f;          // filter cutoff
    float As=30.0f;         // stop-band attenuation [dB]
    float mu=0.0f;          // timing offset
    unsigned int n=64;      // number of random input samples

    // derived values
    unsigned int num_samples = n + h_len;

    // arrays
    float x[num_samples];   // filter input
    float y[num_samples];   // filter output

    unsigned int i;
    float h[h_len];
    liquid_firdes_kaiser(h_len,fc,As,mu,h);
    firfilt_rrrf f = firfilt_rrrf_create(h,h_len);
    firfilt_rrrf_print(f);

    for (i=0; i<num_samples; i++) {
        // generate noise
        x[i] = (i<n) ? randnf()/sqrtf(n/2) : 0.0f;

        firfilt_rrrf_push(f, x[i]);
        firfilt_rrrf_execute(f, &y[i]); 

        printf("x[%4u] = %12.8f, y[%4u] = %12.8f;\n", i, x[i], i, y[i]);
    }   

    // destroy filter object
    firfilt_rrrf_destroy(f);

    // 
    // export results
    //
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% firfilt_rrrf_example.m: auto-generated file\n\n");
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"h_len=%u;\nn=%u;\n", h_len, n);
    
    for (i=0; i<h_len; i++)
        fprintf(fid,"h(%4u) = %12.4e;\n", i+1, h[i]);

    for (i=0; i<num_samples; i++)
        fprintf(fid,"x(%4u) = %12.4e; y(%4u) = %12.4e;\n", i+1, x[i], i+1, y[i]);

    fprintf(fid,"nfft=512;\n");
    fprintf(fid,"H=20*log10(abs(fftshift(fft(h,nfft))));\n");
    fprintf(fid,"X=20*log10(abs(fftshift(fft(x,nfft))));\n");
    fprintf(fid,"Y=20*log10(abs(fftshift(fft(y,nfft))));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,X,'Color',[0.3 0.3 0.3],...\n");
    fprintf(fid,"     f,Y,'LineWidth',2,...\n");
    fprintf(fid,"     f,H,'-b','LineWidth',2);\n");
    fprintf(fid,"axis([-0.5 0.5 -80 40]);\n");
    fprintf(fid,"grid on;\nxlabel('normalized frequency');\nylabel('PSD [dB]');\n");
    fprintf(fid,"legend('noise','filtered noise','filter prototype',1);");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

