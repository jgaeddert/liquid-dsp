//
//
//

#include <stdio.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "firpfbch_synthesis_example.m"

int main() {
    // options
    unsigned int m=4;               // filter delay
    float slsl=-60;                 // sidelobe suppression level
    unsigned int num_symbols=32;    // number of symbols per channel
    unsigned int num_channels=16;   // number of channels
    float gain[16] = {1,1,1,0,1,1,1,0,1,0,1,0,1,1,0,0}; // channel gain vector

    unsigned int num_frames = num_symbols + 2*m;
    unsigned int num_samples = num_frames*num_channels;

    // create synthesizer object
    firpfbch c = firpfbch_create(num_channels, m, slsl, 0, FIRPFBCH_NYQUIST, FIRPFBCH_SYNTHESIZER, 0);

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"num_channels=%u;\n", num_channels);
    fprintf(fid,"num_symbols=%u;\n", num_symbols);
    fprintf(fid,"num_frames=%u;\n", num_frames);
    fprintf(fid,"num_samples=%u;\n", num_samples);

    fprintf(fid,"X = zeros(%u,%u);\n", num_channels, num_frames);
    fprintf(fid,"y = zeros(1,%u);\n",  num_samples);

    unsigned int i, j, n=0;
    float complex X[num_channels];  // channelized symbols
    float complex y[num_channels];  // interpolated time-domain samples
    float g;
    for (i=0; i<num_frames; i++) {

        // generate frame data
        for (j=0; j<num_channels; j++) {
            g = (i<num_symbols) ? gain[j] : 0.0f;
            //X[j] = g*cexpf(_Complex_I*2*M_PI*randf());
            X[j] = g*0.7071f*(randnf() + _Complex_I*randnf());
        }

        // execute synthesis filter bank
        firpfbch_execute(c, X, y);

        // write output to file
        for (j=0; j<num_channels; j++) {
            // frequency data
            fprintf(fid,"X(%4u,%4u) = %12.4e + j*%12.4e;\n",
                    j+1,
                    i+1,
                    crealf(X[j]),
                    cimagf(X[j]));

            // time data
            fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n",
                    n+1,
                    crealf(y[j]),
                    cimag(y[j]));

            // increment output counter
            n++;
        }
    }

    // print results
    fprintf(fid,"\n\n");

    fprintf(fid,"nfft=1024;\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"Y = 20*log10(abs(fft(y,nfft)));\n");
    fprintf(fid,"Y = Y - 10*log10(num_symbols);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,Y,'LineWidth',2,'Color',[0.25 0.5 0.5]);\n");
    fprintf(fid,"xlabel('Normalized Frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"grid on;\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    firpfbch_destroy(c);

    printf("done.\n");
    return 0;
}

