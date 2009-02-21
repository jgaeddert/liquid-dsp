//
//
//

#include <stdio.h>
#include <stdlib.h>
#include "liquid.h"

#define DEBUG_FILENAME  "framegen64_example.m"

int main() {
    // create framegen64 object
    unsigned int m=3;
    float beta=0.7f;
    framegen64 f = framegen64_create(m,beta);

    // data payload
    unsigned char header[24];
    unsigned char payload[64];

    unsigned int i;
    // initialize data
    for (i=0; i<24; i++)
        header[i] = rand() & 0xff;
    for (i=0; i<64; i++)
        payload[i] = rand() & 0xff;

    // generate the frame
    float complex frame[2048];
    framegen64_execute(f, header, payload, frame);

    // write frame to output file
    FILE* fid = fopen(DEBUG_FILENAME, "w");
    fprintf(fid,"%% %s: auto-generated file\n", DEBUG_FILENAME);
    fprintf(fid,"\n\n");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"\n\n");
    for (i=0; i<2048; i++)
        fprintf(fid, "frame(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(frame[i]), cimagf(frame[i]));
    framegen64_flush(f, 64, frame);
    for (i=0; i<64; i++)
        fprintf(fid, "frame(%4u) = %12.4e + j*%12.4e;\n", i+1+2048, crealf(frame[i]), cimagf(frame[i]));

    fprintf(fid,"t=0:(2047+64);\n");
    fprintf(fid,"plot(t,real(frame),t,imag(frame));\n");
    fclose(fid);
    printf("results written to %s\n", DEBUG_FILENAME);

    framegen64_destroy(f);

    printf("done.\n");
    return 0;
}
