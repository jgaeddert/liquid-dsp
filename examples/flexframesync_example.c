//
// flexframesync_example.c
//
// This example demonstrates the basic interface to the flexframegen and
// flexframesync objects used to completely encapsulate raw data bytes
// into frame samples (nearly) ready for over-the-air transmission. A
// 14-byte header and variable length payload are encoded into baseband
// symbols using the flexframegen object.  The resulting symbols are
// interpolated using a root-Nyquist filter and the resulting samples are
// then fed into the flexframesync object which attempts to decode the
// frame. Whenever frame is found and properly decoded, its callback
// function is invoked.
//
// SEE ALSO: flexframesync_advanced_example.c
//           flexframesync_reconfig_example.c
//           framesync64_example.c
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "liquid.h"

// flexframesync callback function
static int callback(unsigned char * _rx_header,
                    int _rx_header_valid,
                    unsigned char * _rx_payload,
                    unsigned int _rx_payload_len,
                    int _rx_payload_valid,
                    framesyncstats_s _stats,
                    void * _userdata);

int main(int argc, char *argv[]) {
    srand( time(NULL) );

    // options
    float SNRdB = 30.0f;            // signal-to-noise ratio
    float noise_floor = -30.0f;     // noise floor
    unsigned int m = 3;             // filter delay
    float beta = 0.7f;              // filter excess bandwidth
    modulation_scheme mod_scheme = LIQUID_MODEM_QPSK;
    unsigned int payload_len = 64;  // payload length

    // derived values
    unsigned int i;
    float nstd  = powf(10.0f, noise_floor/20.0f);         // noise std. dev.
    float gamma = powf(10.0f, (SNRdB+noise_floor)/20.0f); // channel gain

    // create flexframegen object
    flexframegenprops_s fgprops;
    flexframegenprops_init_default(&fgprops);
    fgprops.rampup_len  = 64;
    fgprops.phasing_len = 64;
    fgprops.payload_len = payload_len;
    fgprops.check       = LIQUID_CRC_NONE;
    fgprops.fec0        = LIQUID_FEC_NONE;
    fgprops.fec1        = LIQUID_FEC_NONE;
    fgprops.mod_scheme  = mod_scheme;
    fgprops.rampdn_len  = 64;
    flexframegen fg = flexframegen_create(&fgprops);
    flexframegen_print(fg);

    // frame data (header and payload)
    unsigned char header[14];
    unsigned char payload[fgprops.payload_len];

    // create interpolator
    interp_crcf interp = interp_crcf_create_rnyquist(LIQUID_RNYQUIST_ARKAISER,2,m,beta,0);

    // create flexframesync object with default properties
    framesyncprops_s fsprops;
    framesyncprops_init_default(&fsprops);
    fsprops.squelch_threshold = noise_floor + 3.0f;
    flexframesync fs = flexframesync_create(&fsprops,callback,NULL);
    flexframesync_print(fs);

    // initialize header, payload
    for (i=0; i<14; i++)
        header[i] = i;
    for (i=0; i<fgprops.payload_len; i++)
        payload[i] = rand() & 0xff;

    // generate the frame
    unsigned int frame_len = flexframegen_getframelen(fg);
    float complex frame[frame_len];
    flexframegen_execute(fg, header, payload, frame);

    // interpolate, add noise and push through synchronizer
    float complex y[2]; // interpolator output
    for (i=0; i<frame_len; i++) {
        // run interpolator
        interp_crcf_execute(interp, frame[i], y);

        // apply channel gain
        y[0] *= gamma;
        y[1] *= gamma;

        // add noise
        y[0] += nstd*( randnf() + _Complex_I*randnf())*M_SQRT1_2;
        y[1] += nstd*( randnf() + _Complex_I*randnf())*M_SQRT1_2;

        // push interpolated samples through synchronizer
        flexframesync_execute(fs, y, 2);
    }

    // destroy allocated objects
    flexframegen_destroy(fg);
    flexframesync_destroy(fs);
    interp_crcf_destroy(interp);

    printf("done.\n");
    return 0;
}

static int callback(unsigned char * _rx_header,
                    int _rx_header_valid,
                    unsigned char * _rx_payload,
                    unsigned int _rx_payload_len,
                    int _rx_payload_valid,
                    framesyncstats_s _stats,
                    void * _userdata)
{
    printf("******** callback invoked\n");

    printf("    header crc          : %s\n", _rx_header_valid ?  "pass" : "FAIL");
    printf("    payload length      : %u\n", _rx_payload_len);
    printf("    payload crc         : %s\n", _rx_payload_valid ?  "pass" : "FAIL");
    framesyncstats_print(&_stats);

    return 0;
}

