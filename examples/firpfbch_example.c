//
//
//

#include <stdio.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "firpfbch_example.m"

int main() {
    // options
    unsigned int num_channels=4;    // number of channels
    unsigned int m=5;               // filter delay
    float slsl=-60;                 // sidelobe suppression level
    unsigned int num_symbols=16;    // number of baseband symbols

    unsigned int num_frames = num_symbols+4*m;
    unsigned int num_samples = num_frames*num_channels;
    unsigned int i, j, k, n;

    // create objects
    firpfbch cs = firpfbch_create(num_channels, m, slsl, 0, FIRPFBCH_NYQUIST, FIRPFBCH_SYNTHESIZER);

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"num_channels=%u;\n", num_channels);
    fprintf(fid,"num_frames=%u;\n", num_frames);
    fprintf(fid,"num_samples = num_channels*num_frames;\n");

    fprintf(fid,"x  = zeros(num_channels,num_frames);\n");
    fprintf(fid,"y0 = zeros(1,num_samples);\n");
    fprintf(fid,"y1 = zeros(1,num_samples);\n");

    // generate data buffers
    float complex  x[num_samples];  // input matrix: num_channels x num_frames
    float complex y0[num_samples];  // filterbank channelizer output
    float complex y1[num_samples];  // conventional output

    // objects to run conventional channelizer
    interp_crcf interp[num_channels];
    nco ncox[num_channels];

    // generate filter identical to channelizer's
    // TODO : retrieve filter prototype from channelizer object itself
    float fc = 1.0f/(float)(num_channels);  // cutoff frequency
    unsigned int h_len = 2*m*num_channels;
    float h[h_len+1];
    fir_kaiser_window(h_len+1, fc, slsl, 0.0f, h);

    float f;
    for (i=0; i<num_channels; i++) {
        f = 2.0f * M_PI * (float)(i) / (float)(num_channels);
        printf("f : %12.8f\n", f);
        interp[i] = interp_crcf_create(num_channels, h, h_len);
        ncox[i] = nco_create(LIQUID_VCO);
        nco_set_frequency(ncox[i], f);
    }

    // generate random data
    for (i=0; i<num_samples; i++) {
        x[i] = randnf() + randnf()*_Complex_I;
        // select a specific channel
        //if ((i%num_channels)!=1)
        //    x[i] = 0.0f;
    }

    // synthesize time series
    n=0;
    float complex y0a[num_channels];
    float complex y0b[num_channels];
    for (i=0; i<num_frames; i++) {
        // execute conventional synthesizer
        for (j=0; j<num_channels; j++)
            y0[n+j] = 0.0f;

        for (j=0; j<num_channels; j++) {
            // run interpolator
            interp_crcf_execute(interp[j],x[n+j],y0a);

            // up-convert
            nco_mix_block_up(ncox[j],y0a,y0b,num_channels);

            // append to output buffer
            for (k=0; k<num_channels; k++) {
                y0[n+k] += y0b[k] / (float)(num_channels);
            }
        }

        // execute synthesis filter bank
        firpfbch_execute(cs, &x[n], &y1[n]);

        n += num_channels;
    }

    // write output to file
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"y0(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y0[i]), cimag(y0[i]));
        fprintf(fid,"y1(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y1[i]), cimag(y1[i]));
    }

    n=0;
    // channelized data
    for (i=0; i<num_frames; i++) {
        for (j=0; j<num_channels; j++) {
            fprintf(fid,"x(%4u,%4u) = %12.4e + j*%12.4e;\n", j+1, i+1, crealf(x[n]), cimagf(x[n]));
            n++;
        }
    }

    fprintf(fid,"\n\n");
    fprintf(fid,"t  = 1:num_samples;\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"    plot(t,real(y0), t,real(y1));\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"    plot(t,imag(y0), t,imag(y1));\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    // clean up allocated objects
    for (i=0; i<num_channels; i++) {
        interp_crcf_destroy(interp[i]);
        nco_destroy(ncox[i]);
    }
    firpfbch_destroy(cs);

    printf("done.\n");
    return 0;
}

