//
// iterative tree quadrature mirror filterbank example
//

#include <stdio.h>
#include <complex.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "itqmfb_crcf_analysis_example.m"

int main() {
    unsigned int m=3;                   // filter delay
    float slsl = -60.0f;                // sidelobe suppression level
    unsigned int n = 5;
    unsigned int num_channels = 1<<n;
    unsigned int num_frames = 2*m*n;
    float fc = 0.0f/((float)num_channels);

    // derived values
    unsigned int num_samples = num_channels * num_frames;
    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"n=%u;\n", n);
    fprintf(fid,"x = zeros(1,%u);\n", num_samples);
    fprintf(fid,"y = zeros(%u,%u);\n",num_channels,num_frames);


    // create filterbank
    itqmfb_crcf f = itqmfb_crcf_create(n, m, slsl);
    itqmfb_crcf_print(f);

    float complex x[num_samples];
    float complex y[num_samples];

    unsigned int i,j,t=0;
    for (i=0; i<num_samples; i++) {
        x[i] = cexpf(_Complex_I*2.0f*M_PI*fc*i);
    }

    for (i=0; i<num_frames; i++) {
        itqmfb_crcf_analysis_execute(f,x+i*num_channels,y+i*num_channels);
        for (j=0; j<num_channels; j++) {
            fprintf(fid,"x(1, %3u) = %12.8e + j*%12.8e;\n", t+1, crealf(x[t]),cimagf(x[t]));
            fprintf(fid,"y(%3u,%3u) = %12.8e + j*%12.8e;\n", j+1,i+1, crealf(y[t]), cimagf(y[t]));
            t++;
        }
    }


    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    itqmfb_crcf_destroy(f);
    printf("done.\n");
    return 0;
}
