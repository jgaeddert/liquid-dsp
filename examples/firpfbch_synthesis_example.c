//
//
//

#include <stdio.h>
#include <math.h>

#include "liquid.h"

#define DEBUG_FILENAME "firpfbch_synthesis_example.m"

int main() {
    // options
    float slsl=60;
    unsigned int num_symbols=5;     // num symbols
    unsigned int num_flush=5;       // num symbols to flush buffers

    unsigned int num_channels=8;
    float gain[8] = {1, 1, 1, 0, 1, 0, 0, 1}; // channel gains

    unsigned int num_frames = num_symbols+num_flush;

    // create synthesizer object
    firpfbch c = firpfbch_create(num_channels, slsl, FIRPFBCH_NYQUIST, FIRPFBCH_SYNTHESIZER);

    FILE*fid = fopen(DEBUG_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", DEBUG_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"num_channels=%u;\n", num_channels);
    fprintf(fid,"num_frames=%u;\n", num_frames);

    fprintf(fid,"X = zeros(%u,%u);\n", num_channels, num_frames);
    fprintf(fid,"y = zeros(1,%u);\n",  num_channels * num_frames);

    unsigned int i, j, n=0;
    float complex X[num_channels];  // channelized symbols
    float complex y[num_channels];  // interpolated time-domain samples
    float g;
    for (i=0; i<num_frames; i++) {

        // generate frame data
        for (j=0; j<num_channels; j++) {
            g = (i<num_symbols) ? gain[j] : 0.0f;
            X[j] = g*cexpf(_Complex_I*2*M_PI*randf());
        }

        // execute synthesis filter bank
        firpfbch_execute(c, X, y);

        // write output to file
        for (j=0; j<num_channels; j++) {
            // frequency data
            fprintf(fid,"X(%4u,%4u) = %12.4e + j*%12.4e;\n", j+1, i+1, crealf(X[j]), cimagf(X[j]));

            // time data
            fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", n+1, crealf(y[j]), cimag(y[j]));
            n++;
        }
    }

    // print results
    fprintf(fid,"\n\n");

    fprintf(fid,"nfft=1024;\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"Y = 20*log10(abs(fftshift(fft(y,nfft))));\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,Y,'LineWidth',2,'Color',[0.25 0.5 0.5]);\n");
    fprintf(fid,"xlabel('Normalized Frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"grid on;\n");

    fclose(fid);
    printf("results written to %s\n", DEBUG_FILENAME);

    firpfbch_destroy(c);

    printf("done.\n");
    return 0;
}

