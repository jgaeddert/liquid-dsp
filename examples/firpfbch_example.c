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

    unsigned int i, j, k, n;

    // create objects
    firpfbch cs = firpfbch_create(num_channels, m, slsl, 0, FIRPFBCH_NYQUIST, FIRPFBCH_SYNTHESIZER);

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"num_channels=%u;\n", num_channels);
    fprintf(fid,"num_symbols=%u;\n", num_symbols);
    fprintf(fid,"num_samples = num_channels*num_symbols;\n");

    fprintf(fid,"x  = zeros(num_channels,num_symbols);\n");
    fprintf(fid,"y0 = zeros(1,num_samples);\n");
    fprintf(fid,"y1 = zeros(1,num_samples);\n");

    // generate data buffers
    float complex  x[num_symbols];  // input
    float complex y0[num_symbols];  // conventional output
    float complex y1[num_symbols];  // filterbank channelizer output

    // objects to run conventional channelizer
    interp_crcf interp[num_channels];
    nco ncox[num_channels];

    // retrieve filter taps from channelizer object
    unsigned int h_len = 2*m*num_channels;
    float h[h_len];
    firpfbch_get_filter_taps(cs,h);

    float f;
    for (i=0; i<num_channels; i++) {
        f = 2.0f * M_PI * (float)(i) / (float)(num_channels);
        interp[i] = interp_crcf_create(num_channels, h, h_len);
        ncox[i] = nco_create(LIQUID_VCO);
        nco_set_frequency(ncox[i], f);
    }

    // synthesize time series
    n=0;
    float complex y0a[num_channels];
    float complex y0b[num_channels];
    float rmse_synthesis=0.0f;
    float complex err;
    for (i=0; i<num_symbols; i++) {
        // generate random samples
        for (j=0; j<num_channels; j++) {
            x[j] = randnf() + randnf()*_Complex_I;
            // select a specific channel
            //if ((i%num_channels)!=1)
            //    x[i] = 0.0f;
        }

        // 
        // execute conventional synthesizer
        //
        for (j=0; j<num_channels; j++)
            y0[j] = 0.0f;

        for (j=0; j<num_channels; j++) {
            // run interpolator
            interp_crcf_execute(interp[j],x[j],y0a);

            // up-convert
            nco_mix_block_up(ncox[j],y0a,y0b,num_channels);

            // append to output buffer
            for (k=0; k<num_channels; k++) {
                y0[k] += y0b[k] / (float)(num_channels);
            }
        }

        // 
        // execute synthesis filter bank
        //
        firpfbch_execute(cs, x, y1);

        // compute error
        for (j=0; j<num_channels; j++) {
            err = y0[j] - y1[j];
            rmse_synthesis += crealf(err*conjf(err)) / (float)(num_channels);
        }

        // save output
        for (j=0; j<num_channels; j++) {
            fprintf(fid,"x(%4u,%4u) = %12.4e + j*%12.4e;\n", i+1, j+1, crealf(x[j]), cimag(x[j]));
            fprintf(fid,"y0(%4u) = %12.4e + j*%12.4e;\n", n+j+1, crealf(y0[j]), cimag(y0[j]));
            fprintf(fid,"y1(%4u) = %12.4e + j*%12.4e;\n", n+j+1, crealf(y1[j]), cimag(y1[j]));
        }
        n += num_channels;
    }
    rmse_synthesis = sqrtf(rmse_synthesis);
    printf("synthesis rmse : %12.4e\n", rmse_synthesis);

    fprintf(fid,"\n\n");
    fprintf(fid,"t  = 0:(num_samples-1);\n");
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

