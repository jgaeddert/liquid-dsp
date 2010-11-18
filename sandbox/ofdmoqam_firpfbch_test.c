//
// sandb0x/ofdmoqam_firpfbch_test.c
//
// Tests the validity of OFDM/OQAM using firpfbch_crcf channelizer
// objects.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "liquid.h"

#define OUTPUT_FILENAME "ofdmoqam_firpfbch_test.m"

int main() {
    // options
    unsigned int num_channels=6;    // for now, must be even number
    unsigned int num_symbols=32;    // num symbols
    unsigned int m=3;               // ofdm/oqam symbol delay
    float beta = 0.9f;              // excess bandwidth factor
    modulation_scheme ms = MOD_QAM; // modulation scheme
    unsigned int bps = 2;           // modulation depth (bits/symbol)

    // number of frames (compensate for filter delay)
    unsigned int num_frames = num_symbols + 2*m + 1;

    unsigned int num_samples = num_channels * num_frames;

    // generate transmit filter
    unsigned int h_len = 2*num_channels*m+1;
    float h[h_len];
    design_rkaiser_filter(num_channels,m,beta,0,h);

    // generate receive filter (reversed transmit filter)
    unsigned int g_len = 2*num_channels*m;
    float g[g_len];
    unsigned int i;
    for (i=0; i<g_len; i++)
        g[i] = h[g_len-i-1];

    // create synthesizer/analyzer objects
    firpfbch_crcf cs0 = firpfbch_crcf_create(FIRPFBCH_SYNTHESIZER, num_channels, 2*m, h);
    firpfbch_crcf cs1 = firpfbch_crcf_create(FIRPFBCH_SYNTHESIZER, num_channels, 2*m, h);

    firpfbch_crcf ca0 = firpfbch_crcf_create(FIRPFBCH_ANALYZER, num_channels, 2*m, g);
    firpfbch_crcf ca1 = firpfbch_crcf_create(FIRPFBCH_ANALYZER, num_channels, 2*m, g);

    // modem
    modem mod = modem_create(ms,bps);

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"num_channels=%u;\n", num_channels);
    fprintf(fid,"num_symbols=%u;\n", num_symbols);

    fprintf(fid,"X = zeros(%u,%u);\n", num_channels, num_frames);
    fprintf(fid,"y = zeros(1,%u);\n",  num_samples);
    fprintf(fid,"Y = zeros(%u,%u);\n", num_channels, num_frames);

    unsigned int j, n=0;
    unsigned int k2 = num_channels/2;
    unsigned int s[num_symbols][num_channels];
    float complex X[num_channels];  // channelized symbols
    float complex y[num_channels];  // interpolated time-domain samples
    float complex Y[num_channels];  // received symbols

    // temporary buffers
    float complex X0[num_channels];
    float complex X1[num_channels];
    float complex y0[num_channels];
    float complex y1[num_channels];
    float complex y1_prime[num_channels];
    for (i=0; i<num_channels; i++) y1_prime[i] = 0.0f;

    float complex Z0[num_channels];
    float complex Z1[num_channels];
    float complex z0[num_channels];
    float complex z1[num_channels];
    //float complex z0_prime[num_channels];
    //float complex z1_prime[num_channels];
    for (i=0; i<num_channels; i++) z0[i] = 0.0f;
    for (i=0; i<num_channels; i++) z1[i] = 0.0f;

    // generate random symbols
    for (i=0; i<num_symbols; i++) {
        for (j=0; j<num_channels; j++) {
            s[i][j] = modem_gen_rand_sym(mod);
        }
    }

    for (i=0; i<num_frames; i++) {

        // generate frame data
        for (j=0; j<num_channels; j++) {
            if (i<num_symbols) {
                modem_modulate(mod,s[i][j],&X[j]);
            } else {
                X[j] = 0.0f;
            }
            if (0)
                X[j] = 0.0f;
            else
                X[j] = (rand()%2 ? 1.0f : -1.0f) +
                       (rand()%2 ? 1.0f : -1.0f)*_Complex_I;
        }

        // execute synthesyzer
        //ofdmoqam_execute(cs, X, y);
        for (j=0; j<num_channels; j+=2) {
            // even channels
            X0[j+0] = crealf(X[j+0]);
            X1[j+0] = cimagf(X[j+0])*_Complex_I;

            // odd channels
            X0[j+1] = cimagf(X[j+1])*_Complex_I;
            X1[j+1] = crealf(X[j+1]);
        }
        /*
        for (j=0; j<num_channels; j++)
            printf("X(%3u)  = %12.8f + j*%12.8f\n", j, crealf(X[j]), cimagf(X[j]));
        printf("\n");
        for (j=0; j<num_channels; j++)
            printf("X0(%3u) = %12.8f + j*%12.8f\n", j, crealf(X0[j]), cimagf(X0[j]));
        printf("\n");
        */
        firpfbch_crcf_synthesizer_execute(cs0, X0, y0);
        firpfbch_crcf_synthesizer_execute(cs1, X1, y1);
        // delay lower branch by half a symbol:
        // copy first half of symbol from lower branch
        memmove(&y1_prime[k2], &y1[0], k2*sizeof(float complex));
        // add symbols
        for (j=0; j<num_channels; j++)
            y[j] = y0[j] + y1_prime[j];
        // finish delay
        // copy last half of symbol from lower branch
        memmove(&y1_prime[0], &y1[k2], k2*sizeof(float complex));

        // channel

        // execute analyzer
        //ofdmoqam_execute(ca, y, Y);
        // delay lower branch by half a symbol
        // copy first half of symbol to lower branch
        memmove(&z1[k2], &y[0], k2*sizeof(float complex));
        // run analyzer
        firpfbch_crcf_analyzer_execute(ca0, z0, Z0);
        firpfbch_crcf_analyzer_execute(ca1, z1, Z1);
        // finish delay
        // copy last half of symbol to lower branch
        memmove(&z1[0], &y[k2], k2*sizeof(float complex));
        // copy full symbol on upper branch
        memmove(z0,y,num_channels*sizeof(float complex));
        for (j=0; j<num_channels; j+=2) {
            // even channels
            Y[j+0] = crealf(Z0[j+0]) + cimagf(Z1[j+0])*_Complex_I;

            // odd channels
            Y[j+1] = cimagf(Z0[j+1])*_Complex_I + crealf(Z1[j+1]);
        }

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
    fprintf(fid,"y = y / num_channels; %% normalize by fft size\n");
    fprintf(fid,"for i=1:num_channels,\n");
    fprintf(fid,"    figure;\n");
    fprintf(fid,"    subplot(2,1,1);\n");
    fprintf(fid,"    plot(1:num_symbols,real(x(i,:)),'-x',1:num_symbols,real(y(i,:)),'-x');\n");
    fprintf(fid,"    ylabel('Re');\n");
    fprintf(fid,"    title(['channel ' num2str(i-1)]);\n");
    fprintf(fid,"    subplot(2,1,2);\n");
    fprintf(fid,"    plot(1:num_symbols,imag(x(i,:)),'-x',1:num_symbols,imag(y(i,:)),'-x');\n");
    fprintf(fid,"    ylabel('Im');\n");
    fprintf(fid,"    pause(0.2);\n");
    fprintf(fid,"end;\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    // destroy objects
    firpfbch_crcf_destroy(cs0);
    firpfbch_crcf_destroy(cs1);
    firpfbch_crcf_destroy(ca0);
    firpfbch_crcf_destroy(ca1);
    modem_destroy(mod);

    printf("done.\n");
    return 0;
}

