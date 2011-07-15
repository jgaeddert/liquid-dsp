//
// iterative tree quadrature mirror filterbank example
//

#include <stdio.h>
#include <complex.h>
#include <math.h>

#include "liquid.experimental.h"

#define OUTPUT_FILENAME "itqmfb_rrrf_analysis_example.m"

int main() {
    unsigned int m=3;                   // filter delay
    float As = 60.0f;                   // stop-band attenuation [dB]
    unsigned int n = 4;                 // number of layers
    unsigned int num_channels = 1<<n;
    unsigned int num_frames = 2*m*n;
    float fc = 1.0f/((float)num_channels);

    // derived values
    unsigned int num_samples = num_channels * num_frames;
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"n=%u;\n", n);
    fprintf(fid,"num_samples=%u;\n", num_samples);
    fprintf(fid,"x = zeros(1,%u);\n", num_samples);
    fprintf(fid,"y = zeros(%u,%u);\n",num_channels,num_frames);


    // create filterbank
    itqmfb_rrrf qa = itqmfb_rrrf_create(n, m, As, LIQUID_ITQMFB_ANALYZER);
    itqmfb_rrrf qs = itqmfb_rrrf_create(n, m, As, LIQUID_ITQMFB_SYNTHESIZER);
    itqmfb_rrrf_print(qa);

    float x[num_samples];
    float y[num_samples];
    float z[num_samples];

    unsigned int i,j,t=0;
    for (i=0; i<num_samples; i++) {
        x[i] = cosf(2.0f*M_PI*fc*i);
        x[i] *= (i<128) ? kaiser(i,128,10.0f,0) : 0.0f;
    }

    // execute analyzer
    for (i=0; i<num_frames; i++) {
        itqmfb_rrrf_execute(qa,x+i*num_channels,y+i*num_channels);
    }

    // execute synthesizer
    for (i=0; i<num_frames; i++) {
        itqmfb_rrrf_execute(qs,y+i*num_channels,z+i*num_channels);
    }

    // write results to file
    for (i=0; i<num_frames; i++) {
        for (j=0; j<num_channels; j++) {
            fprintf(fid,"x(1, %3u) = %12.8e;\n",  t+1, x[t]);
            fprintf(fid,"y(%3u,%3u) = %12.8e;\n", j+1,i+1, y[t]);
            fprintf(fid,"z(1, %3u) = %12.8e;\n",  t+1, z[t]);
            t++;
        }
    }

    // plot results
    fprintf(fid,"t=0:(num_samples-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t,x,t,z);\n");
    fprintf(fid,"legend('orginal','reconstructed',0);\n");

    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    itqmfb_rrrf_destroy(qa);
    itqmfb_rrrf_destroy(qs);
    printf("done.\n");
    return 0;
}
