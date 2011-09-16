//
// gmskframesync_example.c [EXPERIMENTAL]
//
// Example demonstrating the OFDM flexible frame synchronizer.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <getopt.h>
#include <time.h>

#include "liquid.h"
#include "liquid.experimental.h"

void usage()
{
    printf("gmskframesync_example [options]\n");
    printf("  u/h   : print usage\n");
    printf("  n     : frame length [bytes], default: 120\n");
    printf("  s     : signal-to-noise ratio [dB], default: 30\n");
}

// callback function
int callback(unsigned char *  _payload,
             unsigned int     _payload_len,
             int              _payload_valid,
             framesyncstats_s _stats,
             void *           _userdata);

int main(int argc, char*argv[])
{
    srand(time(NULL));

    // options
    unsigned int k = 2;
    unsigned int m = 4;
    float BT = 0.5f;
    unsigned int payload_len = 120;     // length of payload (bytes)
    float noise_floor = -60.0f;         // noise floor
    float SNRdB = 30.0f;                // signal-to-noise ratio [dB]

    // get options
    int dopt;
    while((dopt = getopt(argc,argv,"uhn:s:")) != EOF){
        switch (dopt) {
        case 'u':
        case 'h': usage();                      return 0;
        case 'n': payload_len = atol(optarg);   break;
        case 's': SNRdB = atof(optarg);         break;
        default:
            fprintf(stderr,"error: %s, unknown option '%s'\n", argv[0], optarg);
            exit(-1);
        }
    }

    unsigned int i;

    // TODO : validate options

    // derived values
    float nstd = powf(10.0f, noise_floor/20.0f);
    float gamma = powf(10.0f, (SNRdB + noise_floor)/20.0f);

    // allocate memory for payload
    unsigned char payload[payload_len];

    // create frame generator
    gmskframegen fg = gmskframegen_create(k, m, BT);
    gmskframegen_print(fg);

    // create frame synchronizer
    gmskframesync fs = gmskframesync_create(k, m, BT, callback, NULL);
    gmskframesync_print(fs);

    // initialize payload and assemble frame
    for (i=0; i<payload_len; i++) payload[i] = rand() & 0xff;
    gmskframegen_assemble(fg, payload, payload_len);

#if 0
    // initialize frame synchronizer with noise
    for (i=0; i<1000; i++) {
        float complex noise = nstd*( randnf() + _Complex_I*randnf())*M_SQRT1_2;
        gmskframesync_execute(fs, &noise, 1);
    }
#endif

    // generate frame
    unsigned int num_samples = gmskframegen_get_frame_len(fg);
    float complex frame[num_samples];
    unsigned int num_written;
    gmskframegen_write_samples(fg, frame, num_samples, &num_written);

    // push through frame synchronizer
    printf("pushing frame (%u samples) through synchronizer...\n", num_samples);
    gmskframesync_execute(fs, frame, num_samples);

    // destroy objects
    gmskframegen_destroy(fg);
    gmskframesync_destroy(fs);

    printf("done.\n");
    return 0;
}

// callback function
int callback(unsigned char *  _payload,
             unsigned int     _payload_len,
             int              _payload_valid,
             framesyncstats_s _stats,
             void *           _userdata)
{
    //printf("**** callback invoked : rssi = %8.3f dB, evm = %8.3f dB\n", _stats.rssi, _stats.evm);
    printf("**** callback invoked\n");

    return 0;
}

