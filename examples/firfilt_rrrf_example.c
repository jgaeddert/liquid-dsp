//
// firfilt_rrrf_example.c
//

#include <stdio.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "firfilt_rrrf_example.m"

int main() {
    // options
    unsigned int h_len=27;  // filter length
    float fc=0.1f;          // filter cutoff
    float As=60.0f;         // stop-band attenuation [dB]
    float mu=0.0f;          // timing offset
    unsigned int n=200;     // number of random input samples

    // derived values
    unsigned int num_samples = n + h_len;

    // arrays
    float x[num_samples];   // filter input
    float y[num_samples];   // filter output

    unsigned int i;
#if 0
    float h[h_len];
    liquid_firdes_kaiser(h_len,fc,As,mu,h);
    firfilt_rrrf f = firfilt_rrrf_create(h,h_len);
#else
    firfilt_rrrf f = firfilt_rrrf_create_kaiser(h_len,fc,As,mu);
#endif
    firfilt_rrrf_print(f);

    for (i=0; i<num_samples; i++) {
        // generate noise
        x[i] = (i<n) ? randnf() : 0.0f;

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
    fprintf(fid,"num_samples=%u;\n", num_samples);
    fprintf(fid,"x = zeros(1,num_samples);\n");
    fprintf(fid,"y = zeros(1,num_samples);\n");
    
    for (i=0; i<num_samples; i++)
        fprintf(fid,"x(%4u) = %12.4e; y(%4u) = %12.4e;\n", i+1, x[i], i+1, y[i]);

    fprintf(fid,"nfft=512;\n");
    fprintf(fid,"X=20*log10(abs(fftshift(fft(x/sqrt(num_samples),nfft))));\n");
    fprintf(fid,"Y=20*log10(abs(fftshift(fft(y/sqrt(num_samples),nfft))));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"t=[0:(num_samples-1)];\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(t,x,'Color',[0.3 0.3 0.3],...\n");
    fprintf(fid,"       t,y,'LineWidth',2);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('signals');\n");
    fprintf(fid,"  axis([0 num_samples -4 4]);\n");
    fprintf(fid,"  legend('noise','filtered noise',1);");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(f,X,'Color',[0.3 0.3 0.3],...\n");
    fprintf(fid,"       f,Y,'LineWidth',2);\n");
    fprintf(fid,"  grid on;\n");
    fprintf(fid,"  xlabel('normalized frequency');\n");
    fprintf(fid,"  ylabel('PSD [dB]');\n");
    fprintf(fid,"  axis([-0.5 0.5 -80 40]);\n");
    fprintf(fid,"  legend('noise','filtered noise',1);");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

