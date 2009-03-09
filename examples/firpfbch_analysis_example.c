//
//
//

#include <stdio.h>
#include <math.h>

#include "liquid.h"

#define DEBUG_FILENAME "firpfbch_analysis_example.m"

int main() {
    // options
    unsigned int num_channels=8;
    float slsl=60;
    unsigned int num_frames=50;  // num frames

    // create objects
    firpfbch c = firpfbch_create(num_channels, slsl, FIRPFBCH_NYQUIST, FIRPFBCH_ANALYZER);

    //firpfbch_print(c);

    FILE*fid = fopen(DEBUG_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", DEBUG_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"num_channels=%u;\n", num_channels);
    fprintf(fid,"num_frames=%u;\n", num_frames);

    fprintf(fid,"x = zeros(1,%u);\n",  num_channels * num_frames);
    fprintf(fid,"y  = zeros(%u,%u);\n", num_channels, num_frames);

    unsigned int i, j, n=0;
    float complex x[num_channels], y[num_channels];

    // create nco: sweeps entire range of frequencies over the evaluation interval
    nco nco_tx = nco_create();
    nco_set_frequency(nco_tx, 0.0f);
    float df = -2*M_PI/(num_channels*num_frames);
    for (i=0; i<num_frames; i++) {

        // generate frame of data
        for (j=0; j<num_channels; j++) {
            nco_cexpf(nco_tx, &x[j]);
            nco_adjust_frequency(nco_tx, df);
            nco_step(nco_tx);
        }

        // execute analysis filter bank
        firpfbch_execute(c, x, y);

        // write output to file
        for (j=0; j<num_channels; j++) {
            // frequency data
            fprintf(fid,"y(%4u,%4u) = %12.4e + j*%12.4e;\n", j+1, i+1, crealf(y[j]), cimagf(y[j]));

            // time data
            fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", n+1, crealf(x[j]), cimag(x[j]));
            n++;
        }
    }

    // plot results
    fprintf(fid,"\n\n");
    fprintf(fid,"n=min(num_channels,8);\n");
    fprintf(fid,"figure;\n");
    /*
    fprintf(fid,"for i=1:n\n");
    fprintf(fid,"  subplot(n,1,i);\n");
    fprintf(fid,"  plot(abs(y(i,:)));\n");
    fprintf(fid,"  axis off;\n");
    fprintf(fid,"  ylabel(num2str(i));\n");
    fprintf(fid,"end;\n");
    */
    fprintf(fid,"plot(abs(y.'));\n");
    fprintf(fid,"xlabel('time');\n");
    fprintf(fid,"ylabel('channelized energy');\n");


    fclose(fid);
    printf("results written to %s\n", DEBUG_FILENAME);

    nco_destroy(nco_tx);
    firpfbch_destroy(c);

    printf("done.\n");
    return 0;
}

