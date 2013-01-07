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
#include <assert.h>

#include "liquid.h"

// flexframesync callback function
static int callback(unsigned char *  _header,
                    int              _header_valid,
                    unsigned char *  _payload,
                    unsigned int     _payload_len,
                    int              _payload_valid,
                    framesyncstats_s _stats,
                    void *           _userdata);

int main(int argc, char *argv[]) {
    srand( time(NULL) );

    // options
    float SNRdB = 30.0f;            // signal-to-noise ratio
    float noise_floor = -30.0f;     // noise floor
    modulation_scheme mod_scheme = LIQUID_MODEM_QPSK;
    unsigned int payload_len = 64;  // payload length

    // derived values
    unsigned int i;
    float nstd  = powf(10.0f, noise_floor/20.0f);         // noise std. dev.
    float gamma = powf(10.0f, (SNRdB+noise_floor)/20.0f); // channel gain

    // create flexframegen object
    flexframegenprops_s fgprops;
    flexframegenprops_init_default(&fgprops);
    fgprops.check       = LIQUID_CRC_32;
    fgprops.fec0        = LIQUID_FEC_NONE;
    fgprops.fec1        = LIQUID_FEC_HAMMING128;
    fgprops.mod_scheme  = mod_scheme;
    flexframegen fg = flexframegen_create(&fgprops);
    flexframegen_print(fg);

    // frame data (header and payload)
    unsigned char header[14];
    unsigned char payload[payload_len];

    // create flexframesync object with default properties
    framesyncprops_s fsprops;
    framesyncprops_init_default(&fsprops);
    fsprops.squelch_threshold = noise_floor + 3.0f;
    flexframesync fs = flexframesync_create(&fsprops,callback,NULL);
    flexframesync_print(fs);

    // initialize header, payload
    for (i=0; i<14; i++)
        header[i] = i;
    for (i=0; i<payload_len; i++)
        payload[i] = rand() & 0xff;

    // assemble the frame
    flexframegen_assemble(fg, header, payload, payload_len);

    // generate the frame
    unsigned int frame_len = flexframegen_getframelen(fg);
    printf("frame length : %u samples\n", frame_len);
    float complex x[frame_len];
    float complex y[frame_len];

    int frame_complete = 0;
    unsigned int n=0;
    while (!frame_complete) {
        //printf("assert %6u < %6u\n", n, frame_len);
        assert(n < frame_len);
        frame_complete = flexframegen_write_samples(fg, &x[n]);
        n += 2;
    }
    assert(n == frame_len);

    // add noise and push through synchronizer
    for (i=0; i<frame_len; i++) {
        // apply channel gain
        y[i] *= gamma;

        // add noise
        y[i] += nstd*( randnf() + _Complex_I*randnf())*M_SQRT1_2;
    }

    // run through frame synchronizer
    flexframesync_execute(fs, y, frame_len);

    // destroy allocated objects
    flexframegen_destroy(fg);
    flexframesync_destroy(fs);

    printf("done.\n");
    return 0;
}

static int callback(unsigned char *  _header,
                    int              _header_valid,
                    unsigned char *  _payload,
                    unsigned int     _payload_len,
                    int              _payload_valid,
                    framesyncstats_s _stats,
                    void *           _userdata)
{
    printf("******** callback invoked\n");

    printf("    header crc          : %s\n", _header_valid ?  "pass" : "FAIL");
    printf("    payload length      : %u\n", _payload_len);
    printf("    payload crc         : %s\n", _payload_valid ?  "pass" : "FAIL");
    framesyncstats_print(&_stats);

    return 0;
}

