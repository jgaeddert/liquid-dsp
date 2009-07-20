//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "liquid.h"

#define OUTPUT_FILENAME  "flexframesync_example.m"

static int callback(unsigned char * _rx_header,
                    int _rx_header_valid,
                    unsigned char * _rx_payload,
                    unsigned int _rx_payload_len,
                    void * _userdata);

unsigned char header[24];
unsigned char payload[64];

int main() {
    srand( time(NULL) );

    // create flexframegen object
    flexframegenprops_s fgprops;
    fgprops.rampup_len = 16;
    fgprops.phasing_len = 50;
    fgprops.payload_len = 64;
    fgprops.mod_scheme = MOD_PSK;
    fgprops.mod_bps = 3;
    fgprops.rampdn_len = 16;
    flexframegen fg = flexframegen_create(&fgprops);
    flexframegen_print(fg);

    // create interpolator
    unsigned int m=3;
    float beta=0.7f;
    unsigned int h_len = 2*2*m + 1;
    float h[h_len];
    design_rrc_filter(2,m,beta,0,h);
    interp_crcf interp = interp_crcf_create(2,h,h_len);

    // create flexframesync object
    printf("creating flexframesync...\n");
    flexframesyncprops_s fsprops;
    flexframesync fs = flexframesync_create(NULL,callback,NULL);
    printf("done.\n");

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
    unsigned int frame_len = flexframegen_getframelen(fg);
#if 0
    float complex frame_rx[2048];
    
    // push noise
    for (i=0; i<2048; i++) {
        frame_rx[i] = (randnf() + _Complex_I*randnf())*0.01f*gamma;
    }
    flexframesync_execute(fs, frame_rx, 2048);

    flexframegen_execute(fg, header, payload, frame_rx);

    // add channel impairments
    for (i=0; i<2048; i++) {
        frame_rx[i] *= cexpf(_Complex_I*phi);
        frame_rx[i] += (randnf() + _Complex_I*randnf())*0.01f;
        frame_rx[i] *= gamma;
        nco_mix_up(nco_channel, frame_rx[i], &frame_rx[i]);

        nco_step(nco_channel);
    }

    // synchronize/receive the frame
    flexframesync_execute(fs, frame_rx, 2048);

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
#endif

    printf("deleting flexframegen...\n");
    flexframegen_destroy(fg);
    printf("deleting flexframesync...\n");
    flexframesync_destroy(fs);
    nco_destroy(nco_channel);

    printf("done.\n");
    return 0;
}

static int callback(unsigned char * _rx_header,
                    int _rx_header_valid,
                    unsigned char * _rx_payload,
                    unsigned int _rx_payload_len,
                    void * _userdata)
{
    printf("callback invoked\n");

    printf("header crc          : %s\n", _rx_header_valid ?  "pass" : "FAIL");
    //printf("payload crc         : %s\n", _rx_payload_valid ? "pass" : "FAIL");
    printf("payload length      : %u\n", _rx_payload_len);

    // validate payload
    unsigned int i;
    unsigned int num_header_errors=0;
    for (i=0; i<24; i++)
        num_header_errors += (_rx_header[i] == header[i]) ? 0 : 1;
    printf("num header errors   : %u\n", num_header_errors);

    unsigned int num_payload_errors=0;
    for (i=0; i<64; i++)
        num_payload_errors += (_rx_payload[i] == payload[i]) ? 0 : 1;
    printf("num payload errors  : %u\n", num_payload_errors);

    return 0;
}

