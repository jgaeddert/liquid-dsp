//
//
//

#include <stdio.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "firpfbch_example.m"

int main() {
    // options
    unsigned int num_channels=8;    // number of channels
    unsigned int m=5;               // filter delay
    float slsl=-60;                 // sidelobe suppression level
    unsigned int num_symbols=16;    // num frames

    unsigned int num_frames = num_symbols+4*m;
    unsigned int num_samples = num_frames*num_channels;

    // create objects
    firpfbch c0 = firpfbch_create(num_channels, m, slsl, 0, FIRPFBCH_NYQUIST, FIRPFBCH_ANALYZER);
    firpfbch c1 = firpfbch_create(num_channels, m, slsl, 0, FIRPFBCH_NYQUIST, FIRPFBCH_SYNTHESIZER);

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"num_channels=%u;\n", num_channels);
    fprintf(fid,"num_frames=%u;\n", num_frames);

    fprintf(fid,"x0 = zeros(1,%u);\n",  num_samples);
    fprintf(fid,"X  = zeros(%u,%u);\n", num_channels, num_frames);
    fprintf(fid,"x1 = zeros(1,%u);\n",  num_samples);

    // generate data buffers
    float complex x0[num_samples];
    float complex  X[num_samples];
    float complex x1[num_samples];

    unsigned int i, j, n=0;

    // generate Kaiser window
    float beta=10.0f;
    for (i=0; i<num_samples; i++) {
        x0[i] = randnf() + randnf()*_Complex_I;

        if (i<num_symbols*num_channels) {
            x0[i] *= kaiser(i,num_symbols*num_channels,0.0f,beta);
        } else {
            x0[i] *= 0.0f;
        }
    }

    // channelize time series
    for (i=0; i<num_frames; i++) {

        // execute analysis filter bank
        firpfbch_execute(c0, &x0[n], &X[n]);

        // execute synthesis filter bank
        firpfbch_execute(c1, &X[n], &x1[n]);

        n += num_channels;
    }

    // write output to file
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x0(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x0[i]), cimag(x0[i]));
        fprintf(fid,"x1(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x1[i]), cimag(x1[i]));
    }

    n=0;
    for (i=0; i<num_frames; i++) {
        for (j=0; j<num_channels; j++) {
            // frequency data
            fprintf(fid,"X(%4u,%4u) = %12.4e + j*%12.4e;\n", j+1, i+1, crealf(X[n]), cimagf(X[n]));
            n++;
        }
    }

    fprintf(fid,"\n\n");
    fprintf(fid,"t  = %u:%u;\n",    1,  num_symbols*num_channels);
    fprintf(fid,"t0 = t + %u;\n",   0);
    fprintf(fid,"t1 = t + %u;\n",   2*num_channels*m);
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"    plot(t,real(x0(t0)), t, real(x1(t1)));\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"    plot(t,imag(x0(t0)), t, imag(x1(t1)));\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    firpfbch_destroy(c0);
    firpfbch_destroy(c1);

    printf("done.\n");
    return 0;
}

