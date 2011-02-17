//
// firpfbch_analysis_example.c
//
// Example of the analysis channelizer filterbank. The input signal is a
// frequency-modulated sweep over the entire band. Each filter is
// illuminated as the carrier passes through its bandwidth.
//

#include <stdio.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "firpfbch_analysis_example.m"

int main() {
    // options
    unsigned int num_channels=8;    // number of channels
    unsigned int m=2;               // filter delay
    float As=-60;                   // stop-band attenuation
    unsigned int num_frames=25;     // number of frames

    // create objects
    firpfbch_crcf c = firpfbch_crcf_create_kaiser(LIQUID_ANALYZER, num_channels, m, As);

    //firpfbch_crcf_print(c);

    FILE*fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all;\nclose all;\n\n");
    fprintf(fid,"num_channels=%u;\n", num_channels);
    fprintf(fid,"num_frames=%u;\n", num_frames);

    fprintf(fid,"x = zeros(1,%u);\n",  num_channels * num_frames);
    fprintf(fid,"y  = zeros(%u,%u);\n", num_channels, num_frames);

    unsigned int i, j, n=0;
    float complex x[num_channels];  // time-domain input
    float complex y[num_channels];  // channelized output

    // create nco: sweeps entire range of frequencies over the evaluation interval
    nco_crcf nco_tx = nco_crcf_create(LIQUID_VCO);
    nco_crcf_set_frequency(nco_tx, 0.0f);
    float df = 2*M_PI/(num_channels*num_frames);
    printf("fr/ch:");
    for (j=0; j<num_channels; j++)  printf("%3u",j);
    printf("\n");
    for (i=0; i<num_frames; i++) {

        // generate frame of data
        for (j=0; j<num_channels; j++) {
            nco_crcf_cexpf(nco_tx, &x[j]);
            nco_crcf_adjust_frequency(nco_tx, df);
            nco_crcf_step(nco_tx);
        }

        // execute analysis filter bank
        firpfbch_crcf_analyzer_execute(c, x, y);

        printf("%4u : ", i);
        for (j=0; j<num_channels; j++) {
            if (cabsf(y[j]) > num_channels / 4)
                printf(" x ");
            else
                printf(" . ");
        }
        printf("\n");

        // write output to file
        for (j=0; j<num_channels; j++) {
            // frequency data
            fprintf(fid,"y(%4u,%4u) = %12.4e + j*%12.4e;\n", j+1, i+1, crealf(y[j]), cimagf(y[j]));

            // time data
            fprintf(fid,"x(%4u) = %12.4e + j*%12.4e;\n", n+1, crealf(x[j]), cimag(x[j]));
            n++;
        }
    }

    // destroy objects
    nco_crcf_destroy(nco_tx);
    firpfbch_crcf_destroy(c);

    // plot results
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"subplot(2,1,1);\n");
    fprintf(fid,"  plot(1:length(x),real(x),1:length(x),imag(x));\n");
    fprintf(fid,"  xlabel('time');\n");
    fprintf(fid,"  ylabel('signal');\n");
    fprintf(fid,"subplot(2,1,2);\n");
    fprintf(fid,"  plot(20*log10(abs(y.')/num_channels));\n");
    fprintf(fid,"  xlabel('time (decimated)');\n");
    fprintf(fid,"  ylabel('channelized energy [dB]');\n");

    fprintf(fid,"n=min(num_channels,8);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"for i=1:n\n");
    fprintf(fid,"  subplot(n,1,i);\n");
    fprintf(fid,"  plot(1:num_frames,real(y(i,:)),1:num_frames,imag(y(i,:)));\n");
    fprintf(fid,"  axis off;\n");
    fprintf(fid,"  ylabel(num2str(i));\n");
    fprintf(fid,"end;\n");


    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

