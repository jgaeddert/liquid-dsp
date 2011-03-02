#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <liquid/liquid.h>

// static callback function
static int callback(unsigned char * _header,
                    int _header_valid,
                    unsigned char * _payload,
                    int _payload_valid,
                    framesyncstats_s _stats,
                    void * _userdata);

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

    unsigned char header[12];
    unsigned char payload[64];

    // data payload
    unsigned int i;
    // initialize header, payload
    for (i=0; i<12; i++)
        header[i] = i;
    for (i=0; i<64; i++)
        payload[i] = rand() & 0xff;

    // allocate memory for the frame samples
    float complex frame_rx[1280];
    
#if 0
    // push noise (flush the frame buffers)
    for (i=0; i<1280; i++) {
        frame_rx[i] = (randnf() + _Complex_I*randnf())*0.01f*gamma;
    }
    framesync64_execute(fs, frame_rx, 1280);
#endif

    // generate the frame
    framegen64_execute(fg, header, payload, frame_rx);

    // add channel impairments
    for (i=0; i<1280; i++) {
        frame_rx[i] *= cexpf(_Complex_I*phi);
        frame_rx[i] *= gamma;
        frame_rx[i] += (randnf() + _Complex_I*randnf()) * 0.707f * nstd;
        nco_crcf_mix_up(nco_channel, frame_rx[i], &frame_rx[i]);

        nco_crcf_step(nco_channel);
    }

    // synchronize/receive the frame
    framesync64_execute(fs, frame_rx, 1280);

    // clean up allocated objects
    framegen64_destroy(fg);
    framesync64_destroy(fs);
    nco_crcf_destroy(nco_channel);

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
    printf("***** callback invoked!\n");
    return 0;
}

