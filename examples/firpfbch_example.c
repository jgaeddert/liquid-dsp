//
//
//

#include <stdio.h>
#include <math.h>

#include "liquid.h"

#define DEBUG_FILENAME "firpfbch_example.m"

int main() {
    // options
    unsigned int num_channels=8;    // number of channels
    unsigned int m=2;               // filter delay
    float slsl=-60;                 // sidelobe suppression level
    unsigned int num_frames=25;     // num frames

    // create objects
    firpfbch c0 = firpfbch_create(num_channels, m, slsl, FIRPFBCH_NYQUIST, FIRPFBCH_ANALYZER);
    firpfbch c1 = firpfbch_create(num_channels, m, slsl, FIRPFBCH_NYQUIST, FIRPFBCH_SYNTHESIZER);

    //firpfbch_print(c0);

    FILE*fid = fopen(DEBUG_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", DEBUG_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"num_channels=%u;\n", num_channels);
    fprintf(fid,"num_frames=%u;\n", num_frames);

    fprintf(fid,"x0 = zeros(1,%u);\n",  num_channels * num_frames);
    fprintf(fid,"X  = zeros(%u,%u);\n", num_channels, num_frames);
    fprintf(fid,"x1 = zeros(1,%u);\n",  num_channels * num_frames);

    unsigned int i, j, n=0;
    float complex x0[num_channels], X[num_channels], x1[num_channels];

    // create nco: sweeps entire range of frequencies over the evaluation interval
    nco nco_tx = nco_create();
    nco_set_frequency(nco_tx, -M_PI);
    float df = 2*M_PI/(num_channels*num_frames);
    for (i=0; i<num_frames; i++) {

        // generate frame of data
        for (j=0; j<num_channels; j++) {
            nco_cexpf(nco_tx, &x0[j]);
            nco_adjust_frequency(nco_tx, df);
            nco_step(nco_tx);
        }

        // execute analysis filter bank
        firpfbch_execute(c0, x0, X);

        // execute synthesis filter bank
        firpfbch_execute(c1, X, x1);

        // write output to file
        for (j=0; j<num_channels; j++) {
            // frequency data
            fprintf(fid,"X(%4u,%4u) = %12.4e + j*%12.4e;\n", j+1, i+1, crealf(X[j]), cimagf(X[j]));

            // time data
            fprintf(fid,"x0(%4u) = %12.4e + j*%12.4e;\n", n+1, crealf(x0[j]), cimag(x0[j]));
            fprintf(fid,"x1(%4u) = %12.4e + j*%12.4e;\n", n+1, crealf(x1[j]), cimag(x1[j]));
            n++;
        }
    }   

    fclose(fid);
    printf("results written to %s\n", DEBUG_FILENAME);

    nco_destroy(nco_tx);
    firpfbch_destroy(c0);
    firpfbch_destroy(c1);

    printf("done.\n");
    return 0;
}

