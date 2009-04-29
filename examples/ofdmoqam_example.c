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
    float slsl=60;
    unsigned int num_symbols=5;     // num symbols
    unsigned int num_flush=5;       // num symbols to flush buffers

    unsigned int num_channels=8;    // for now, must be even number
    float gain[8] = {1,1,1,1,1,1,1,1};

    unsigned int num_frames = num_symbols+num_flush;

    // create synthesizer objects
    firpfbch cs0 = firpfbch_create(num_channels, slsl, FIRPFBCH_ROOTNYQUIST, FIRPFBCH_SYNTHESIZER);
    firpfbch cs1 = firpfbch_create(num_channels, slsl, FIRPFBCH_ROOTNYQUIST, FIRPFBCH_SYNTHESIZER);

    // create analyzer objects
    firpfbch ca0 = firpfbch_create(num_channels, slsl, FIRPFBCH_ROOTNYQUIST, FIRPFBCH_ANALYZER);
    firpfbch ca1 = firpfbch_create(num_channels, slsl, FIRPFBCH_ROOTNYQUIST, FIRPFBCH_ANALYZER);

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

    float complex X0[num_channels]; // even channel input
    float complex X1[num_channels]; // odd channel input
    float complex Xi, Xq;

    float complex y0[num_channels]; // even channel output
    float complex y1[num_channels]; // odd channel output
    float complex y_prime[num_channels];
    for (i=0; i<num_channels; i++)
        y_prime[i] = 0;

    float g;
    for (i=0; i<num_frames; i++) {

        // generate frame data
        for (j=0; j<num_channels; j++) {
            g = (i<num_symbols) ? gain[j] : 0.0f;
            s = modem_gen_rand_sym(mod);
            modem_modulate(mod,s,&X[j]);
            X[j] *= g;
        }

        // prepare signal
        for (j=0; j<num_channels; j++) {
            Xi = crealf(X[j]);
            Xq = cimagf(X[j])*_Complex_I;
            if ((j%2)==0) { // even channel
                X0[j] = Xi;
                X1[j] = Xq;
            } else {        // odd channel
                X0[j] = Xq;
                X1[j] = Xi;
            }
        }

        // execute synthesis filter banks
        firpfbch_execute(cs0, X0, y0);
        firpfbch_execute(cs1, X1, y1);
        memmove(&y_prime[num_channels/2],y1,(num_channels/2)*sizeof(float complex));
        for (j=0; j<num_channels; j++)
            y[j] = y0[j] + y_prime[j];
        memmove(&y_prime[0],             y1,(num_channels/2)*sizeof(float complex));


        //
        // channel
        //

        // execute analysis filter banks
        firpfbch_execute(ca0, y, Y);
        firpfbch_execute(ca1, y, Y);

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
    firpfbch_destroy(cs0);
    firpfbch_destroy(cs1);
    firpfbch_destroy(ca0);
    firpfbch_destroy(ca1);
    modem_destroy(mod);

    printf("done.\n");
    return 0;
}

