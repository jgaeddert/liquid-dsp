//
// orthogonal frequency-divisional multiplexing
// offset quadrature amplitude modulation (ofdm/oqam)
// synchronization example.
//

#include <stdio.h>
#include <math.h>
#include <string.h>

#include "liquid.h"

#define DEBUG_FILENAME "ofdmoqam_sync_example.m"

int main() {
    // options
    unsigned int num_channels=32;   // must be even number
    unsigned int num_symbols=4;     // num symbols (needs to be even)
    unsigned int m=2;               // ofdm/oqam symbol delay
    modulation_scheme ms = MOD_QAM; // modulation scheme
    unsigned int bps = 4;           // modulation depth (bits/symbol)
    float df=0.1*M_PI;              // carrier offset
    float noise_std=0.01f;          // noise standard deviation

    // number of frames (compensate for filter delay, add some
    // excess for plotting purposes)
    unsigned int num_frames = 2*num_symbols + 2*m + 1;

    unsigned int num_samples = num_channels * num_frames;

    // create synthesizer/analyzer objects
    ofdmoqam cs = ofdmoqam_create(num_channels, m, OFDMOQAM_SYNTHESIZER);
    ofdmoqam ca = ofdmoqam_create(num_channels, m, OFDMOQAM_ANALYZER);

    // modem
    modem mod = modem_create(ms,bps);

    // channel nco
    nco nco_channel = nco_create();
    nco_set_frequency(nco_channel, df);

    FILE*fid = fopen(DEBUG_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", DEBUG_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"num_channels=%u;\n", num_channels);
    fprintf(fid,"num_symbols=%u;\n", num_symbols);
    fprintf(fid,"num_samples=%u;\n", num_samples);

    fprintf(fid,"X = zeros(%u,%u);\n", num_channels, num_frames);
    fprintf(fid,"y = zeros(1,%u);\n",  num_samples);
    fprintf(fid,"Y = zeros(%u,%u);\n", num_channels, num_frames);

    unsigned int i, j, n=0;
    unsigned int s[num_symbols][num_channels];
    float complex X[num_channels];  // channelized symbols
    float complex y[num_channels];  // interpolated time-domain samples
    float complex Y[num_channels];  // received symbols

    // delay lines
    unsigned int d=num_channels*num_symbols/2;
    cfwindow w0 = cfwindow_create(3*d);
    cfwindow w1 = cfwindow_create(2*d);
    float complex * r0;
    float complex * r1;
    float complex rxx[num_channels];

    // generate random symbols
    for (i=0; i<num_symbols/2; i++) {
        for (j=0; j<num_channels; j++) {
            s[i][j] = modem_gen_rand_sym(mod);
        }
    }

    // repeat symbols (time-domain correlation)
    for (i=0; i<num_symbols/2; i++) {
        for (j=0; j<num_channels; j++) {
            s[i + num_symbols/2][j] = s[i][j];
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
        }

        // execute synthesyzer
        ofdmoqam_execute(cs, X, y);

        // channel
        for (j=0; j<num_channels; j++) {
            // add carrier offset
            nco_mix_up(nco_channel, y[j], &y[j]);
            nco_step(nco_channel);

            // add noise
            cawgn(&y[j],noise_std);
        }

        // execute delay
        for (j=0; j<num_channels; j++) {
            cfwindow_push(w0,     y[j] );
            cfwindow_push(w1,conj(y[j]));

            cfwindow_read(w0,&r0);
            cfwindow_read(w1,&r1);
            dotprod_cccf_run(r0,r1,2*d,&rxx[j]);
        }

        // execute analyzer
        ofdmoqam_execute(ca, y, Y);

        // write output to file
        for (j=0; j<num_channels; j++) {
            // frequency data
            fprintf(fid,"X(%4u,%4u) = %12.4e + j*%12.4e;\n", j+1, i+1, crealf(X[j]), cimagf(X[j]));

            // time data
            fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", n+1, crealf(y[j]), cimag(y[j]));
            fprintf(fid,"rxx(%4u) = %12.4e + j*%12.4e;\n", n+1, crealf(rxx[j]), cimag(rxx[j]));

            // received data
            fprintf(fid,"Y(%4u,%4u) = %12.4e + j*%12.4e;\n", j+1, i+1, crealf(Y[j]), cimagf(Y[j]));
            n++;
        }
    }

    // print results
    fprintf(fid,"\n\n");

    fprintf(fid,"t = 0:(num_samples-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"    plot(t,real(y),t,imag(y));\n");
    fprintf(fid,"    ylabel('signal');\n");
    fprintf(fid,"    legend('in-phase','quadrature',1);\n");
    //fprintf(fid,"    grid on;\n");
    fprintf(fid,"subplot(2,1,2);\n");
    //fprintf(fid,"    plot(t,abs(rxx),'LineWidth',2);\n");
    fprintf(fid,"    plot(t,abs(rxx));\n");
    fprintf(fid,"    ylabel('cross-correlation');\n");
    //fprintf(fid,"    grid on;\n");

    fclose(fid);
    printf("results written to %s\n", DEBUG_FILENAME);

    // destroy objects
    ofdmoqam_destroy(cs);
    ofdmoqam_destroy(ca);
    modem_destroy(mod);
    nco_destroy(nco_channel);

    cfwindow_destroy(w0);
    cfwindow_destroy(w1);

    printf("done.\n");
    return 0;
}

