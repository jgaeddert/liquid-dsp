//
// ofdmflexframesync_example.c
//
// Example demonstrating the OFDM flexible frame synchronizer.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "liquid.h"

#define OUTPUT_FILENAME "ofdmframesync_example.m"

static int callback(void * _userdata);

int main(int argc, char*argv[]) {
    srand(time(NULL));

    // options
    unsigned int M = 64;                // number of subcarriers
    unsigned int cp_len = 16;           // cyclic prefix length
    unsigned int payload_len = 120;     //
#if 0
    modulation_scheme ms = LIQUID_MODEM_QAM;
    unsigned int bps = 4;
    fec_scheme fec0 = LIQUID_FEC_NONE;
    fec_scheme fec1 = LIQUID_FEC_NONE;
    crc_scheme crc  = LIQUID_CRC_32;
    float noise_floor = -30.0f;         // noise floor [dB]
    float SNRdB = 20.0f;                // signal-to-noise ratio [dB]
#endif

    // TODO : validate options

    // derived values
    unsigned int frame_len = M + cp_len;
    float complex buffer[frame_len]; // time-domain buffer
#if 0
    float nstd = powf(10.0f, noise_floor/10.0f);
    float gamma = powf(10.0f, (SNRdB + noise_floor)/10.0f);
#endif

    // initialize subcarrier allocation
    unsigned int p[M];
    ofdmframe_init_default_sctype(M, p);

    // create frame generator
    ofdmflexframegenprops_s fgprops;
    ofdmflexframegenprops_init_default(&fgprops);
    fgprops.num_symbols_S0  = 3;
    fgprops.payload_len     = payload_len;
    fgprops.check           = LIQUID_CRC_32;
    fgprops.fec0            = LIQUID_FEC_NONE;
    fgprops.fec1            = LIQUID_FEC_HAMMING128;
    fgprops.mod_scheme      = LIQUID_MODEM_QPSK;
    fgprops.mod_bps         = 2;
    ofdmflexframegen fg = ofdmflexframegen_create(M, cp_len, p, &fgprops);
    ofdmflexframegen_print(fg);

    // create frame synchronizer
    ofdmflexframesync fs = ofdmflexframesync_create(M, cp_len, p, callback, NULL);
    ofdmflexframesync_print(fs);

    // initialize header, payload
    unsigned char header[1];
    unsigned char payload[payload_len];

    unsigned int i;
    for (i=0; i<payload_len; i++)
        payload[i] = rand() & 0xff;

    // assemble frame
    ofdmflexframegen_assemble(fg, header, payload);

    float nstd = 0.01f;
    float complex noise;
#if 1
    // initialize frame synchronizer with noise
    for (i=0; i<1000; i++) {
        noise = nstd * randnf() * cexpf(_Complex_I*M_PI*randf());
        ofdmflexframesync_execute(fs, &noise, 1);
    }
#endif

    // generate frame
    int last_symbol=0;
    unsigned int num_written;
    while (!last_symbol) {
        // generate symbol
        last_symbol = ofdmflexframegen_writesymbol(fg, buffer, &num_written);

        // TODO : apply channel
        for (i=0; i<num_written; i++) {
            noise = nstd * randnf() * cexpf(_Complex_I*M_PI*randf());
            buffer[i] += noise;
        }

        // receive symbol
        ofdmflexframesync_execute(fs, buffer, num_written);
    }

    // destroy objects
    ofdmflexframegen_destroy(fg);
    ofdmflexframesync_destroy(fs);

    printf("done.\n");
    return 0;
}

static int callback(void * _userdata)
{
    printf("**** callback invoked\n");

    return 0;
}

