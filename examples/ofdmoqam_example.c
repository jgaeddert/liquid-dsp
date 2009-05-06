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
    unsigned int num_channels=14;    // for now, must be even number
    unsigned int num_symbols=32;    // num symbols
    unsigned int m=2;               // ofdm/oqam symbol delay
    modulation_scheme ms = MOD_QAM; // modulation scheme
    unsigned int bps = 4;           // modulation depth (bits/symbol)

    // number of frames (compensate for filter delay)
    unsigned int num_frames = num_symbols + 2*m;

    unsigned int num_samples = num_channels * num_frames;

    // create synthesizer/analyzer objects
    ofdmoqam cs = ofdmoqam_create(num_channels, m, OFDMOQAM_SYNTHESIZER);
    ofdmoqam ca = ofdmoqam_create(num_channels, m, OFDMOQAM_ANALYZER);

    // modem
    modem mod = modem_create(ms,bps);

    modem mod0 = modem_create(MOD_QAM,4);
    modem mod1 = modem_create(MOD_QPSK,2);

    FILE*fid = fopen(DEBUG_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", DEBUG_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"num_channels=%u;\n", num_channels);
    fprintf(fid,"num_symbols=%u;\n", num_symbols);

    fprintf(fid,"X = zeros(%u,%u);\n", num_channels, num_frames);
    fprintf(fid,"y = zeros(1,%u);\n",  num_samples);
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
            g = (i<num_symbols) ? 1.0f : 0.0f;
            s = modem_gen_rand_sym(mod);
            modem_modulate(mod,s,&X[j]);

#if 0
            if ((j%2)==0) {
                s = modem_gen_rand_sym(mod0);
                modem_modulate(mod0,s,&X[j]);
            } else {
                s = modem_gen_rand_sym(mod1);
                modem_modulate(mod1,s,&X[j]);
            }
#endif

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
    fprintf(fid,"x = X(:,1:%u);\n", num_symbols);
    fprintf(fid,"y = Y(:,%u:%u);\n", 2*m+1, num_symbols + 2*m);
    fprintf(fid,"for i=1:num_channels,\n");
    fprintf(fid,"    figure;\n");
    fprintf(fid,"    subplot(2,1,1);\n");
    fprintf(fid,"    plot(1:num_symbols,real(x(i,:)),'-x',1:num_symbols,real(y(i,:)),'-x');\n");
    fprintf(fid,"    title(['channel ' num2str(i-1)]);\n");
    fprintf(fid,"    subplot(2,1,2);\n");
    fprintf(fid,"    plot(1:num_symbols,imag(x(i,:)),'-x',1:num_symbols,imag(y(i,:)),'-x');\n");
    fprintf(fid,"    pause(0.2);\n");
    fprintf(fid,"end;\n");

    fclose(fid);
    printf("results written to %s\n", DEBUG_FILENAME);

    // destroy objects
    ofdmoqam_destroy(cs);
    ofdmoqam_destroy(ca);
    modem_destroy(mod);

    modem_destroy(mod0);
    modem_destroy(mod1);

    printf("done.\n");
    return 0;
}

