//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"

#define OUTPUT_FILENAME "cvsd_sine_example.m"

int main() {
    // options
    unsigned int n=256;
    unsigned int nbits=3;
    float zeta=1.5f;

    // create cvsd codecs
    cvsd cvsd_encoder = cvsd_create(nbits, zeta);
    cvsd cvsd_decoder = cvsd_create(nbits, zeta);

    // open debug file
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");

    fprintf(fid,"n=%u;\n", n);
    fprintf(fid,"x=zeros(1,n);\n");
    fprintf(fid,"y=zeros(1,n);\n");

    float phi=0.0f;
    float dphi=0.1f;
    unsigned int i;
    unsigned char b;
    float x,y;
    float rmse=0.0f;
    for (i=0; i<n; i++) {
        x = 0.5f*sinf(phi);
        b = cvsd_encode(cvsd_encoder, x);
        y = cvsd_decode(cvsd_decoder, b);

        printf("%1u ", b);
        if ( ((i+1)%16) == 0 )
            printf("\n");

        fprintf(fid,"x(%3u) = %12.4e;\n", i+1, x);
        fprintf(fid,"y(%3u) = %12.4e;\n", i+1, y);

        rmse += (x-y)*(x-y);
        phi += dphi;
    }

    rmse = sqrtf(rmse/n);
    printf("\n");
    printf("signal/distortion: %8.2f dB\n", -20*log10f(rmse));

    // plot results
    fprintf(fid,"\n\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(1:n,x,1:n,y);\n");
    fprintf(fid,"xlabel('time [sample index]');\n");
    fprintf(fid,"ylabel('signal');\n");
    fprintf(fid,"legend('audio input','cvsd output',1);\n");

    // close debug file
    fclose(fid);
    printf("results wrtten to %s\n", OUTPUT_FILENAME);
    printf("done.\n");
    return 0;
}

