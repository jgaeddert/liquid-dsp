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

int callback(unsigned char *  _header,
             int              _header_valid,
             unsigned char *  _payload,
             unsigned int     _payload_len,
             int              _payload_valid,
             framesyncstats_s _stats,
             void *           _userdata);

int main(int argc, char*argv[]) {
    srand(time(NULL));

    // options
    unsigned int M = 64;                // number of subcarriers
    unsigned int cp_len = 16;           // cyclic prefix length
    unsigned int payload_len = 120;     //
    modulation_scheme ms = LIQUID_MODEM_QPSK;
    unsigned int bps = 2;
    fec_scheme fec0  = LIQUID_FEC_NONE;
    fec_scheme fec1  = LIQUID_FEC_HAMMING128;
    crc_scheme check = LIQUID_CRC_32;
#if 0
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
    fgprops.check           = check;
    fgprops.fec0            = fec0;
    fgprops.fec1            = fec1;
    fgprops.mod_scheme      = ms;
    fgprops.mod_bps         = bps;
    ofdmflexframegen fg = ofdmflexframegen_create(M, cp_len, p, &fgprops);
    ofdmflexframegen_print(fg);

    // create frame synchronizer
    ofdmflexframesync fs = ofdmflexframesync_create(M, cp_len, p, callback, NULL);
    ofdmflexframesync_print(fs);

    // initialize header, payload
    unsigned char header[8];
    unsigned char payload[payload_len];

    unsigned int i;

    for (i=0; i<8; i++)
        header[i] = i & 0xff;
    for (i=0; i<payload_len; i++)
        payload[i] = rand() & 0xff;

    // assemble frame
    ofdmflexframegen_assemble(fg, header, payload);

    float nstd = 0.01f;
    float complex noise;
#if 1
    // initialize frame synchronizer with noise
    for (i=0; i<1000; i++) {
        noise = nstd * randnf() * cexpf(_Complex_I*2*M_PI*randf());
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
            noise = nstd * randnf() * cexpf(_Complex_I*2*M_PI*randf());
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

int callback(unsigned char *  _header,
             int              _header_valid,
             unsigned char *  _payload,
             unsigned int     _payload_len,
             int              _payload_valid,
             framesyncstats_s _stats,
             void *           _userdata)
{
    printf("**** callback invoked\n");

    unsigned int i;

    printf("  header rx  :");
    for (i=0; i<8; i++)
        printf(" %.2X", _header[i]);
    printf("\n");

    printf("  payload rx :");
    for (i=0; i<_payload_len; i++) {
        printf(" %.2X", _payload[i]);
        if ( ((i+1)%26)==0 && i !=_payload_len-1 )
            printf("\n              ");
    }
    printf("\n");

    return 0;
}

