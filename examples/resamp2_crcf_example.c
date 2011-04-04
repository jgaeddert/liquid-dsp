//
// resamp2_crcf_example.c
//
// Halfband interpolator|decimator.
//

#include <stdio.h>
#include <complex.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "resamp2_crcf_example.m"

int main() {
    unsigned int m=5;               // filter semi-length
    unsigned int h_len = 4*m+1;     // filter length
    float bw=0.25f;                 // input signal bandwidth
    unsigned int num_samples=64;    // number of input samples
    float As=60.0f;                 // stop-band attenuation [dB]

    unsigned int i;

    // allocate memory for data arrays
    float complex x[  num_samples];
    float complex y[2*num_samples];
    float complex z[  num_samples];

    // generate the baseband signal (filter pulse)
    unsigned int t = num_samples - h_len;
    t += t % 2 ? 0 : 1; // ensure 't' is odd
    float h[t];
    firdes_kaiser_window(t,bw,60.0f,0.0f,h);
    for (i=0; i<num_samples; i++)
        x[i] = i < t ? h[i]*bw : 0.0f;

    // create/print the half-band resampler, with a specified
    // stopband attenuation level
    resamp2_crcf q = resamp2_crcf_create(h_len,0,As);
    resamp2_crcf_print(q);

    // run interpolation stage
    for (i=0; i<num_samples; i++)
        resamp2_crcf_interp_execute(q, x[i], &y[2*i]);

    // clear resamp2 object
    resamp2_crcf_clear(q);

    // execute decimation stage
    for (i=0; i<num_samples; i++)
        resamp2_crcf_interp_execute(q, y[2*i], &z[i]);

    // clean up allocated objects
    resamp2_crcf_destroy(q);

    // 
    // print results to file
    //
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n",OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n\n");
    fprintf(fid,"h_len=%u;\n", h_len);
    fprintf(fid,"n=%u;\n", num_samples);

    // output results
    for (i=0; i<num_samples; i++)
        fprintf(fid,"x(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimagf(x[i]));

    for (i=0; i<2*num_samples; i++)
        fprintf(fid,"y(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));

    for (i=0; i<num_samples; i++)
        fprintf(fid,"z(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(z[i]), cimagf(z[i]));

    // print results
    fprintf(fid,"\n\n");
    fprintf(fid,"nfft=1024;\n");
    fprintf(fid,"f = [0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"X = 20*log10(abs(fftshift(fft(x,  nfft))));\n");
    fprintf(fid,"Y = 20*log10(abs(fftshift(fft(y/2,nfft))));\n");
    fprintf(fid,"Z = 20*log10(abs(fftshift(fft(z,  nfft))));\n");
    fprintf(fid,"plot(f,X,f,Y,f,Z);\n");
    fprintf(fid,"legend('original','up-converted','down-converted',1);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"axis([-0.5 0.5 -120 20]);\n");

    fprintf(fid,"\n\n");
    fprintf(fid,"t0 = 0:[n-1];\n");
    fprintf(fid,"t1 = 0:[n*2-1];\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(3,1,1);\n");
    fprintf(fid,"  plot(t0,real(x),'-s','MarkerSize',3,t0,imag(x),'-s','MarkerSize',3);\n");
    fprintf(fid,"  legend('I','Q',0);\n");
    fprintf(fid,"  axis([0 n -0.25 0.55]);\n");
    fprintf(fid,"  ylabel('original');\n");
    fprintf(fid,"subplot(3,1,2);\n");
    fprintf(fid,"  plot(t1,real(y),t1,imag(y));\n");
    fprintf(fid,"  axis([0 n*2 -0.25 0.55]);\n");
    fprintf(fid,"  ylabel('interpolated');\n");
    fprintf(fid,"subplot(3,1,3);\n");
    fprintf(fid,"  plot(t0,real(z),'-s','MarkerSize',3,t0,imag(z),'-s','MarkerSize',3);\n");
    fprintf(fid,"  axis([0 n -0.25 0.55]);\n");
    fprintf(fid,"  ylabel('interp/decim');\n");


    fclose(fid);
    printf("results written to %s\n",OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}
