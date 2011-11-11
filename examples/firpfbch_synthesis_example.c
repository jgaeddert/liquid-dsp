//
// firpfbch_synthesis_example.c
//
// Example of the synthesis channelizer filterbank.  Random symbols are
// generated and loaded into the bins of the channelizer and the time-
// domain signal is synthesized.  Subcarriers around the band edges are
// disabled as well as those near 0.25 to demonstrate the synthesizer's
// ability to efficiently notch the spectrum. The results are printed to
// a file for plotting.
//

#include <stdio.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "firpfbch_synthesis_example.m"

int main() {
    // options
    unsigned int m=4;               // filter delay
    float As=-60;                   // stop-band attenuation
    unsigned int num_symbols=1;     // number of symbols per channel
    unsigned int num_channels=32;   // number of channels

    unsigned int i;
    unsigned int num_frames = num_symbols + 2*m;
    unsigned int num_samples = num_frames*num_channels;

    // compute channel gain, nominally 1, but 0 at band edges and
    // notched at 0.25
    float gain[num_channels]; // channel gain vector
    unsigned int M2 = num_channels / 2;     // channel center
    unsigned int G  = num_channels / 6;     // guard band
    unsigned int N  = num_channels / 5;     // null center
    for (i=0; i<num_channels; i++) {
        if (i > M2 - G && i < M2 + G)
            gain[i] = 0;
        else if (i > N && i < N+3)
            gain[i] = 0;
        else
            gain[i] = 1;

        printf("gain[%3u] = %12.8f\n", i, gain[i]);
    }

    // create synthesizer object
    firpfbch_crcf c = firpfbch_crcf_create_kaiser(LIQUID_SYNTHESIZER, num_channels, m, As);

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"num_channels=%u;\n", num_channels);
    fprintf(fid,"num_symbols=%u;\n", num_symbols);
    fprintf(fid,"num_frames=%u;\n", num_frames);
    fprintf(fid,"num_samples=%u;\n", num_samples);

    fprintf(fid,"X = zeros(%u,%u);\n", num_channels, num_frames);
    fprintf(fid,"y = zeros(1,%u);\n",  num_samples);

    unsigned int j, n=0;
    float complex X[num_channels];  // channelized symbols
    float complex y[num_channels];  // interpolated time-domain samples
    float g;
    for (i=0; i<num_frames; i++) {

        // generate frame data
        for (j=0; j<num_channels; j++) {
            g = (i<num_symbols) ? gain[j] : 0.0f;

            if (num_symbols == 1) {
                // set symbol to unity gain
                X[j] = g;
            } else  {
                // set random symbol
                X[j] = g*0.7071f*(randnf() + _Complex_I*randnf());
            }
        }

        // execute synthesis filter bank
        firpfbch_crcf_synthesizer_execute(c, X, y);

        // normalize signal level
        for (j=0; j<num_channels; j++)
            y[j] /= num_channels;

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
    fprintf(fid,"Y = 20*log10(abs(fftshift(fft(y,nfft))));\n");
    fprintf(fid,"Y = Y - 10*log10(num_symbols);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,Y,'LineWidth',2,'Color',[0.25 0.5 0.5]);\n");
    fprintf(fid,"xlabel('Normalized Frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"grid on;\n");
    fprintf(fid,"axis([-0.5 0.5 -100 20]);\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    firpfbch_crcf_destroy(c);

    printf("done.\n");
    return 0;
}

