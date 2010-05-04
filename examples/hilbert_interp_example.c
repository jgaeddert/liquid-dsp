//
// firhilb_interp_example.c
//
// Hilbert transform: 1:2 complex-to-real interpolator.  This
// example demonstrates the functionality of firhilb (finite
// impulse response Hilbert transform) interpolator which converts
// a complex time series into a real one with twice the number of
// samples.  The input is a complex-valued sinusoid of N samples.
// The output is a real-valued sinusoid of 2*N samples.
//
// SEE ALSO: hilbert_decim_example.c

#include <stdio.h>
#include <complex.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "hilbert_interp_example.m"

int main() {
    unsigned int m=5;           // filter semi-length
    unsigned int h_len = 4*m+1; // filter length
    float slsl=60.0f;           // filter sidelobe suppression level
    float fc=0.73f;             // signal center frequency
    unsigned int N=128;         // number of samples

    // create Hilbert transform object
    firhilb f = firhilb_create(h_len,slsl);
    firhilb_print(f);

    // open output file
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"h_len=%u;\nN=%u;\n", h_len, N);

    unsigned int i;
    float y[2], theta=0.0f, dtheta=2*M_PI*fc;
    float complex x;
    for (i=0; i<N; i++) {
        // compute complex input signal
        x = cexpf(_Complex_I*theta);
        theta += dtheta;

        // execute transform (interpolator) to compute real signal
        firhilb_interp_execute(f, x, y);

        // print results
        fprintf(fid,"x(%3u) = %8.4f + j*%8.4f;\n", i+1, crealf(x), cimagf(x));
        fprintf(fid,"y(%3u) = %8.4f;\n", 2*i+1, y[0]);
        fprintf(fid,"y(%3u) = %8.4f;\n", 2*i+2, y[1]);

        printf("y(%3u) = %8.4f;\n", 2*i+1, y[0]);
        printf("y(%3u) = %8.4f;\n", 2*i+2, y[1]);
    }

    // plot results
    fprintf(fid,"nfft=512;\n");
    fprintf(fid,"%% compute normalized windowing functions\n");
    fprintf(fid,"wx = 1.8534/N*hamming(length(x)).'; %% x window\n");
    fprintf(fid,"wy = 1.8534/N*hamming(length(y)).'; %% y window\n");
    fprintf(fid,"X=20*log10(abs(        (fft(x.*wx,nfft))));\n");
    fprintf(fid,"Y=20*log10(abs(fftshift(fft(y.*wy,nfft))));\n");
    fprintf(fid,"f =[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"fi=[0:(nfft-1)]/(2*nfft);\n");
    fprintf(fid,"figure; plot(fi,X,'Color',[0.5 0.5 0.5],f,Y,'LineWidth',2);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"axis([-0.5 0.5 -80 20]);\n");
    fprintf(fid,"xlabel('normalized frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"legend('original/complex','transformed/interpolated',1);");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    firhilb_destroy(f);
    printf("done.\n");
    return 0;
}
