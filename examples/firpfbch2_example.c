//
// firpfbch2_example.c
//
// Example of the finite impulse response (FIR) polyphase filterbank
// (PFB) channelizer with an output rate of 2 Fs / M as an (almost)
// perfect reconstructive system.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "liquid.h"

#define OUTPUT_FILENAME "firpfbch2_example.m"

int main() {
    // options
    unsigned int num_channels=6;    // number of channels
    unsigned int m = 4;             // filter semi-length (symbols)
    unsigned int num_symbols=4*m;   // number of symbols
    float As = 81.29528f;           // filter stop-band attenuation
    
    unsigned int i;

    // validate input

    // derived values
    unsigned int num_samples = num_channels * num_symbols;

    // allocate arrays
    float complex x[num_samples];
    float complex y[num_samples];

    // generate input signal
    for (i=0; i<num_samples; i++)
        x[i] = (i==0) ? 1.0f : 0.0f;

    // create filterbank objects from prototype
    firpfbch2_crcf qa = firpfbch2_crcf_create_kaiser(LIQUID_ANALYZER,    num_channels, m, As);
    firpfbch2_crcf qs = firpfbch2_crcf_create_kaiser(LIQUID_SYNTHESIZER, num_channels, m, As);

    // run channelizer
    float complex Y[num_channels];
    for (i=0; i<num_samples; i+=num_channels/2) {
        // run analysis filterbank
        firpfbch2_crcf_execute(qa, &x[i], Y);

        // print
        unsigned int j;
        printf("i = %u\n", i);
        for (j=0; j<num_channels; j++)
            printf("  Y[%3u] = %12.8f + %12.8fj\n", j+1, crealf(Y[j]), cimagf(Y[j]));

        // apply frequency-band filtering/selection

        // run synthesis filterbank
        firpfbch2_crcf_execute(qs, Y, &y[i]);
    }

    // destroy fiterbank objects
    firpfbch2_crcf_destroy(qa); // analysis fitlerbank
    firpfbch2_crcf_destroy(qs); // synthesis filterbank

    //
    // EXPORT DATA TO FILE
    //
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"num_channels=%u;\n", num_channels);
    fprintf(fid,"num_symbols=%u;\n",  num_symbols);
    fprintf(fid,"num_samples = num_channels*num_symbols;\n");

    fprintf(fid,"x = zeros(1,num_samples);\n");
    fprintf(fid,"y = zeros(1,num_samples);\n");

    // save input and output arrays
    for (i=0; i<num_samples; i++) {
        fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(x[i]), cimag(x[i]));
        fprintf(fid,"y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimag(y[i]));
    }

    // plot results
    fprintf(fid,"t  = 0:(num_samples-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"title('composite');\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"    plot(t,real(x), t,imag(x));\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"    plot(t,real(y), t,imag(y));\n");

    fclose(fid);
    printf("results written to '%s'\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}
