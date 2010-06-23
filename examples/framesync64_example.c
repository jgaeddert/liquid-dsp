//
// framegen64_example.c
//
// This example demonstrates the interfaces to the framegen64 and
// framesync64 objects used to completely encapsulate data for
// over-the-air transmission.  A 24-byte header and 64-byte payload are
// encoded, modulated, and interpolated using the framegen64 object.
// The resulting complex baseband samples are corrupted with noise and
// moderate carrier frequency and phase offsets before the framesync64
// object attempts to decode the frame.  The resulting data are compared
// to the original to validate correctness.
//
// SEE ALSO: flexframesync_example.c
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "liquid.h"

#define OUTPUT_FILENAME  "framesync64_example.m"

// static callback function
static int callback(unsigned char * _header,
                    int _header_valid,
                    unsigned char * _payload,
                    int _payload_valid,
                    framesyncstats_s _stats,
                    void * _userdata);

// global header, payload arrays
unsigned char header[24];
unsigned char payload[64];

int main() {
    srand( time(NULL) );

    // options
    float SNRdB = 10.0f;
    float noise_floor = -40.0f;

    // create framegen64 object
    unsigned int m=3;
    float beta=0.7f;
    
    // create frame generator
    framegen64 fg = framegen64_create(m,beta);

    // create frame synchronizer using default properties
    framesync64 fs = framesync64_create(NULL,callback,NULL);
    framesync64_print(fs);

    // channel
    float phi=0.3f;
    float dphi=0.05f;
    float nstd  = powf(10.0f, noise_floor/10.0f);         // noise std. dev.
    float gamma = powf(10.0f, (SNRdB+noise_floor)/10.0f); // channel gain
    nco_crcf nco_channel = nco_crcf_create(LIQUID_VCO);
    nco_crcf_set_phase(nco_channel, phi);
    nco_crcf_set_frequency(nco_channel, dphi);

    // data payload
    unsigned int i;
    // initialize header, payload
    for (i=0; i<24; i++)
        header[i] = i;
    for (i=0; i<64; i++)
        payload[i] = rand() & 0xff;

    // allocate memory for the frame samples
    float complex frame_rx[2048];
    
#if 0
    // push noise (flush the frame buffers)
    for (i=0; i<2048; i++) {
        frame_rx[i] = (randnf() + _Complex_I*randnf())*0.01f*gamma;
    }
    framesync64_execute(fs, frame_rx, 2048);
#endif

    // generate the frame
    framegen64_execute(fg, header, payload, frame_rx);

    // add channel impairments
    for (i=0; i<2048; i++) {
        frame_rx[i] *= cexpf(_Complex_I*phi);
        frame_rx[i] *= gamma;
        frame_rx[i] += (randnf() + _Complex_I*randnf()) * 0.707f * nstd;
        nco_crcf_mix_up(nco_channel, frame_rx[i], &frame_rx[i]);

        nco_crcf_step(nco_channel);
    }

    // synchronize/receive the frame
    framesync64_execute(fs, frame_rx, 2048);

    // clean up allocated objects
    framegen64_destroy(fg);
    framesync64_destroy(fs);
    nco_crcf_destroy(nco_channel);

    // write frame to output file
    FILE* fid = fopen(OUTPUT_FILENAME, "w");
    fprintf(fid,"%% %s: auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"\n\n");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"\n\n");
    for (i=0; i<2048; i++)
        fprintf(fid, "frame_rx(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(frame_rx[i]), cimagf(frame_rx[i]));

    fprintf(fid,"t=0:2047;\n");
    fprintf(fid,"plot(t,real(frame_rx),t,imag(frame_rx));\n");
    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

// static callback function
static int callback(unsigned char * _rx_header,
                    int _rx_header_valid,
                    unsigned char * _rx_payload,
                    int _rx_payload_valid,
                    framesyncstats_s _stats,
                    void * _userdata)
{
    printf("*** callback invoked ***\n");
    printf("    SNR                 : %12.8f dB\n", _stats.SNR);
    printf("    rssi                : %12.8f dB\n", _stats.rssi);

    printf("    header crc          : %s\n", _rx_header_valid ?  "pass" : "FAIL");
    printf("    payload crc         : %s\n", _rx_payload_valid ? "pass" : "FAIL");

    // validate payload
    unsigned int i;
    unsigned int num_header_errors=0;
    for (i=0; i<24; i++)
        num_header_errors += (_rx_header[i] == header[i]) ? 0 : 1;
    printf("    num header errors   : %u\n", num_header_errors);

    unsigned int num_payload_errors=0;
    for (i=0; i<64; i++)
        num_payload_errors += (_rx_payload[i] == payload[i]) ? 0 : 1;
    printf("    num payload errors  : %u\n", num_payload_errors);

    return 0;
}

