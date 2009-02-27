//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "liquid.h"

#define DEBUG_FILENAME  "framesync64_example.m"

static int callback(unsigned char * _header, unsigned char * _payload);

unsigned char header[24];
unsigned char payload[64];

int main() {
    srand( time(NULL) );
    // create framegen64 object
    unsigned int m=3;
    float beta=0.7f;
    framegen64 fg = framegen64_create(m,beta);
    framesync64 fs = framesync64_create(m,beta,callback);

    // channel
    float phi=0.3f;
    float dphi=0.0f;
    float gamma=0.1f;  // channel gain
    nco nco_channel = nco_create();
    nco_set_phase(nco_channel, phi);
    nco_set_frequency(nco_channel, dphi);

    // data payload
    unsigned int i;
    // initialize header, payload
    for (i=0; i<24; i++)
        header[i] = i;
    for (i=0; i<64; i++)
        payload[i] = rand() & 0xff;

    // generate the frame
    float complex frame_rx[2048];
    
    // push noise
    for (i=0; i<2048; i++) {
        frame_rx[i] = (randnf() + _Complex_I*randnf())*0.01f*gamma;
    }
    framesync64_execute(fs, frame_rx, 2048);

    framegen64_execute(fg, header, payload, frame_rx);

    // add channel impairments
    for (i=0; i<2048; i++) {
        frame_rx[i] *= cexpf(_Complex_I*phi);
        frame_rx[i] += (randnf() + _Complex_I*randnf())*0.01f;
        frame_rx[i] *= gamma;
        nco_mix_up(nco_channel, frame_rx[i], &frame_rx[i]);

        nco_step(nco_channel);
    }

    // synchronize/receive the frame
    framesync64_execute(fs, frame_rx, 2048);

    // write frame to output file
    FILE* fid = fopen(DEBUG_FILENAME, "w");
    fprintf(fid,"%% %s: auto-generated file\n", DEBUG_FILENAME);
    fprintf(fid,"\n\n");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"\n\n");
    for (i=0; i<2048; i++)
        fprintf(fid, "frame_rx(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(frame_rx[i]), cimagf(frame_rx[i]));

    fprintf(fid,"t=0:2047;\n");
    fprintf(fid,"plot(t,real(frame_rx),t,imag(frame_rx));\n");
    fclose(fid);
    printf("results written to %s\n", DEBUG_FILENAME);

    framegen64_destroy(fg);
    framesync64_destroy(fs);
    nco_destroy(nco_channel);

    printf("done.\n");
    return 0;
}

static int callback(unsigned char * _rx_header, unsigned char * _rx_payload)
{
    printf("callback invoked\n");

    // validate payload
    unsigned int i;
    unsigned int num_header_errors=0;
    for (i=0; i<24; i++)
        num_header_errors += (_rx_header[i] == header[i]) ? 0 : 1;
    printf("num header errors:  %u\n", num_header_errors);

    unsigned int num_payload_errors=0;
    for (i=0; i<64; i++)
        num_payload_errors += (_rx_payload[i] == payload[i]) ? 0 : 1;
    printf("num payload errors: %u\n", num_payload_errors);

    return 0;
}

