//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.h"

#define DEBUG_FILENAME "fbasc_example.m"

int main() {
    // options
    unsigned int num_channels=32;
    unsigned int samples_per_frame=1024;
    unsigned int bytes_per_frame=256;

    unsigned int num_frames=4;

    // create fbasc codecs
    fbasc fbasc_encoder = fbasc_create(FBASC_ENCODER, num_channels, samples_per_frame, bytes_per_frame);
    //fbasc fbasc_decoder = fbasc_create(FBASC_DECODER, num_channels, samples_per_frame, bytes_per_frame);

    // open debug file
    FILE * fid = fopen(DEBUG_FILENAME,"w");
    fprintf(fid,"%% %s: auto-generated file\n\n", DEBUG_FILENAME);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");

    float phi=0.0f;
    float dphi;
    unsigned int i, j;
    float x[samples_per_frame];
    unsigned char framedata[bytes_per_frame];
    for (i=0; i<num_frames; i++) {
        dphi = 0.2f;
        for (j=0; j<samples_per_frame; j++) {
            x[j] = 0.5f*sinf(phi);
            phi += dphi;
        }

        fbasc_encode(fbasc_encoder, x, framedata);

        phi += dphi;
    }

    // plot results
    fprintf(fid,"\n\n");

    // close debug file
    fclose(fid);
    printf("results wrtten to %s\n", DEBUG_FILENAME);
    printf("done.\n");
    return 0;
}

