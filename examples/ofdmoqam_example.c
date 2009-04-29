//
//
//

#include <stdio.h>
#include <math.h>
#include <string.h>

#include "liquid.h"

#define DEBUG_FILENAME "ofdmoqam_example.m"

int main() {
    // options
    unsigned int num_symbols=32;    // num symbols
    unsigned int num_flush=4;       // num symbols to flush buffers
    unsigned int m=2;               // ofdm/oqam symbol delay

    unsigned int num_channels=4;    // for now, must be even number
    float gain[4] = {1,1,1,1};

    unsigned int num_frames = num_symbols+num_flush;

    // create synthesizer/analyzer objects
    ofdmoqam cs = ofdmoqam_create(num_channels, m, OFDMOQAM_SYNTHESIZER);
    ofdmoqam ca = ofdmoqam_create(num_channels, m, OFDMOQAM_ANALYZER);

    // modem
    modem mod = modem_create(MOD_QAM,4);

    FILE*fid = fopen(DEBUG_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", DEBUG_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"num_channels=%u;\n", num_channels);
    fprintf(fid,"num_symbols=%u;\n", num_symbols);

    fprintf(fid,"X = zeros(%u,%u);\n", num_channels, num_frames);
    fprintf(fid,"y = zeros(1,%u);\n",  num_channels * num_frames);
    fprintf(fid,"Y = zeros(%u,%u);\n", num_channels, num_frames);

    unsigned int i, j, n=0;
    unsigned int s;
    float complex X[num_channels];  // channelized symbols
    float complex y[num_channels];  // interpolated time-domain samples
    float complex Y[num_channels];  // received symbols

    float g;
    for (i=0; i<num_frames; i++) {

        // generate frame data
        for (j=0; j<num_channels; j++) {
            g = (i<num_symbols) ? gain[j] : 0.0f;
            s = modem_gen_rand_sym(mod);
            modem_modulate(mod,s,&X[j]);
            X[j] *= g;
        }

        // execute synthesyzer
        ofdmoqam_execute(cs, X, y);

        // channel

        // execute analyzer
        ofdmoqam_execute(ca, y, Y);

        // write output to file
        for (j=0; j<num_channels; j++) {
            // frequency data
            fprintf(fid,"X(%4u,%4u) = %12.4e + j*%12.4e;\n", j+1, i+1, crealf(X[j]), cimagf(X[j]));

            // time data
            fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", n+1, crealf(y[j]), cimag(y[j]));

            // received data
            fprintf(fid,"Y(%4u,%4u) = %12.4e + j*%12.4e;\n", j+1, i+1, crealf(Y[j]), cimagf(Y[j]));
            n++;
        }
    }

    // print results
    fprintf(fid,"\n\n");
    fprintf(fid,"return\n");
    fprintf(fid,"nfft=1024;\n");
    fprintf(fid,"f=[0:(nfft-1)]/nfft - 0.5;\n");
    fprintf(fid,"Y = 20*log10(abs(fft(y,nfft)));\n");
    fprintf(fid,"Y = Y - 20*log10(num_channels * num_symbols);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,Y,'LineWidth',2,'Color',[0.25 0.5 0.5]);\n");
    fprintf(fid,"xlabel('Normalized Frequency');\n");
    fprintf(fid,"ylabel('PSD [dB]');\n");
    fprintf(fid,"grid on;\n");

    fclose(fid);
    printf("results written to %s\n", DEBUG_FILENAME);

    // destroy objects
    ofdmoqam_destroy(cs);
    ofdmoqam_destroy(ca);
    modem_destroy(mod);

    printf("done.\n");
    return 0;
}

