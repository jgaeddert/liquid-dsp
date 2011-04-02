//
// resamp2_crcf_decim_example.c
//
// Halfband decimator.  This example demonstrates the interface to the
// decimating halfband resampler.  A low-frequency input sinusoid is
// generated and fed into the decimator two samples at a time,
// producing one output at each iteration.  The results are written to
// an output file.
//
// SEE ALSO: resamp2_crcf_interp_example.c
//           decim_rrrf_example.c
//

#include <stdio.h>
#include <complex.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "resamp2_crcf_decim_example.m"

int main() {
    unsigned int m=5;           // filter semi-length
    float fc=0.17f;             // input tone frequency
    unsigned int N=128;         // number of output samples
    float As=60.0f;             // stop-band attenuation [dB]

    // create/print the half-band resampler, centered on
    // tone frequency with a specified stop-band attenuation
    resamp2_cccf f = resamp2_cccf_create(m,fc,As);
    resamp2_cccf_print(f);

    // open output file
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"h_len=%u;\n", 4*m+1);
    fprintf(fid,"N=%u;\n", N);

    unsigned int i;
    float theta=0.0f, dtheta=2*M_PI*fc;
    float complex x[2], y;
    for (i=0; i<N; i++) {
        // generate input : complex sinusoid
        x[0] = cexpf(_Complex_I*theta);
        theta += dtheta;
        x[1] = cexpf(_Complex_I*theta);
        theta += dtheta;

        // run the decimator
        resamp2_cccf_decim_execute(f, x, &y);

        // save results to output file
        fprintf(fid,"x(%3u) = %8.4f + j*%8.4f;\n", 2*i+1, crealf(x[0]), cimagf(x[0]));
        fprintf(fid,"x(%3u) = %8.4f + j*%8.4f;\n", 2*i+2, crealf(x[1]), cimagf(x[1]));
        fprintf(fid,"y(%3u) = %8.4f + j*%8.4f;\n", i+1,   crealf(y),    cimagf(y));

        printf("y(%3u) = %8.4f + j*%8.4f;\n", i+1, crealf(y), cimagf(y));
    }

    fprintf(fid,"nfft=512;\n");
    fprintf(fid,"wx = hamming(length(x))'/N*1.8678/2;   %% normalized window (input)\n");
    fprintf(fid,"wy = hamming(length(y))'/N*1.8678/2;   %% normalized window (output)\n");
    fprintf(fid,"X=20*log10(abs(fftshift(fft(x.*wx,nfft))));\n");
    fprintf(fid,"Y=20*log10(abs(fftshift(fft(y.*wy,nfft))));\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft-0.5;\n");
    fprintf(fid,"figure; plot(f,X,'Color',[0.5 0.5 0.5],f/2,Y,'LineWidth',2);\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"xlabel('normalized frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"legend('original','decimated',1);");
    fprintf(fid,"axis([-0.5 0.5 -100 10]);\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    // clean up allocated objects
    resamp2_cccf_destroy(f);
    printf("done.\n");
    return 0;
}
