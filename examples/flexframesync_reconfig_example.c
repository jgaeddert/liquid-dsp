//
// flexframesync_reconfig_example.c
//
// Demonstrates the reconfigurability of the flexframegen and
// flexframesync objects.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <getopt.h>

#include "liquid.h"

#define OUTPUT_FILENAME  "flexframesync_reconfig_example.m"

void usage()
{
    printf("flexframesync_example [options]\n");
    printf("  u/h   : print usage\n");
    printf("  v/q   : verbose/quiet output\n");
    printf("  s     : signal-to-noise ratio [dB], default: 30\n");
    printf("  n     : number of frames, default: 3\n");
}

// flexframesync callback function
static int callback(unsigned char * _rx_header,
                    int _rx_header_valid,
                    unsigned char * _rx_payload,
                    unsigned int _rx_payload_len,
                    int _rx_payload_valid,
                    framesyncstats_s _stats,
                    void * _userdata);

typedef struct {
    unsigned char * header;
    unsigned char * payload;
    unsigned int num_frames_received;
    unsigned int num_payloads_decoded;
} framedata;

// global verbose flag
int verbose = 1;

int main(int argc, char *argv[]) {
    srand( time(NULL) );

    // define parameters
    float SNRdB = 30.0f;
    unsigned int m = 3;     // filter delay
    float beta = 0.7f;      // filter excess bandwidth
    float noise_floor = -30.0f;
    unsigned int num_frames = 3;

    // get options
    int dopt;
    while((dopt = getopt(argc,argv,"uhvqs:f:m:p:n:")) != EOF){
        switch (dopt) {
        case 'u':
        case 'h': usage();                      return 0;
        case 'v': verbose=1;                    break;
        case 'q': verbose=0;                    break;
        case 's': SNRdB = atof(optarg);         break;
        case 'n': num_frames = atoi(optarg);    break;
        default:
            exit(1);
        }
    }

    // create flexframegen object
    flexframegenprops_s fgprops;
    flexframegenprops_init_default(&fgprops);
    flexframegen fg = flexframegen_create(NULL);

    // frame data
    unsigned char header[14];
    unsigned char * payload = NULL;
    framedata fd = {NULL, NULL, 0, 0};

    // create interpolator
    interp_crcf interp = interp_crcf_create_rnyquist(LIQUID_RNYQUIST_RRC,2,m,beta,0);

    // create flexframesync object with default properties
    framesyncprops_s fsprops;
    framesyncprops_init_default(&fsprops);
    fsprops.squelch_threshold = noise_floor + 3.0f;
#if 0
    // override default properties
    fsprops.agc_bw0 = 1e-3f;
    fsprops.agc_bw1 = 1e-5f;
    fsprops.agc_gmin = 1e-3f;
    fsprops.agc_gmax = 1e4f;
    fsprops.pll_bw0 = 1e-1f;
    fsprops.pll_bw1 = 1e-2f;
#endif
    flexframesync fs = flexframesync_create(&fsprops,callback,(void*)&fd);

    // channel
    float phi=0.0f;
    float dphi=0.02f;
    nco_crcf nco_channel = nco_crcf_create(LIQUID_VCO);
    nco_crcf_set_phase(nco_channel, phi);
    nco_crcf_set_frequency(nco_channel, dphi);
    float nstd  = powf(10.0f, noise_floor/20.0f);         // noise std. dev.
    float gamma = powf(10.0f, (SNRdB+noise_floor)/20.0f); // channel gain
    float mu    = 0.3f; // fractional sample delay
    firfarrow_crcf delay_filter = firfarrow_crcf_create(27,5,0.45f,60.0f);
    firfarrow_crcf_set_delay(delay_filter,mu);

    unsigned int i;
    // initialize header, payload
    for (i=0; i<14; i++)
        header[i] = i;

    // frame buffers, properties
    unsigned int frame_len;
    float complex * frame = NULL;

    // interpolate, push through synchronizer
    float complex x;
    float complex y[2];
    float complex z[2];
    float complex noise;
    for (i=0; i<512; i++) {
        noise = 0.0f;
        cawgn(&noise, nstd);
        // push noise through sync
        flexframesync_execute(fs, &noise, 1);
    }

    unsigned int j;
    for (j=0; j<num_frames; j++) {
        // configure frame generator properties
        fgprops.rampup_len  = 64;
        fgprops.phasing_len = 64;
        fgprops.payload_len = (rand() % 256) + 1;   // random payload length
        fgprops.check       = LIQUID_CRC_NONE;      // data validity check
        fgprops.fec0        = LIQUID_FEC_NONE;      // inner FEC scheme
        fgprops.fec1        = LIQUID_FEC_NONE;      // outer FEC scheme
        fgprops.mod_scheme  = LIQUID_MODEM_PSK;     // PSK
        fgprops.mod_bps     = (rand() % 4) + 1;     // random bits/symbol
        fgprops.rampdn_len  = 64;

        // set properties
        flexframegen_setprops(fg, &fgprops);
        if (verbose)
            flexframegen_print(fg);

        // reallocate memory for payload
        payload = realloc(payload, fgprops.payload_len*sizeof(unsigned char));

        // initialize payload
        for (i=0; i<fgprops.payload_len; i++)
            payload[i] = rand() & 0xff;

        // compute frame length
        frame_len = flexframegen_getframelen(fg);

        // reallocate memory for frame
        frame = realloc(frame, frame_len*sizeof(float complex));

        // set framedata pointers
        fd.header = header;
        fd.payload = payload;

        // generate the frame
        flexframegen_execute(fg, header, payload, frame);

        for (i=0; i<frame_len+2*m; i++) {
            // compensate for filter delay
            x = (i<frame_len) ? frame[i] : 0.0f;

            // run interpolator
            interp_crcf_execute(interp, x, y);

            // add channel impairments
            nco_crcf_mix_up(nco_channel, y[0], &z[0]);
            nco_crcf_step(nco_channel);
            nco_crcf_mix_up(nco_channel, y[1], &z[1]);
            nco_crcf_step(nco_channel);

            // apply channel gain
            z[0] *= gamma;
            z[1] *= gamma;

            // add noise
            cawgn(&z[0], nstd);
            cawgn(&z[1], nstd);

            // emulate sample timing offset with Farrow filter
            firfarrow_crcf_push(delay_filter, z[0]);
            firfarrow_crcf_execute(delay_filter, &z[0]);
            firfarrow_crcf_push(delay_filter, z[1]);
            firfarrow_crcf_execute(delay_filter, &z[1]);

            // push through sync
            flexframesync_execute(fs, z, 2);
        }

        // flush frame
        for (i=0; i<200; i++) {
            noise = 0.0f;
            cawgn(&noise, nstd);
            // push noise through sync
            interp_crcf_execute(interp, noise, y);
            firfarrow_crcf_push(delay_filter, y[0]);
            firfarrow_crcf_execute(delay_filter, &z[0]);
            firfarrow_crcf_push(delay_filter, y[1]);
            firfarrow_crcf_execute(delay_filter, &z[1]);
            flexframesync_execute(fs, z, 2);
        }
    } // num frames

    printf("num frames received  : %3u / %3u\n", fd.num_frames_received,  num_frames);
    printf("num payloads decoded : %3u / %3u\n", fd.num_payloads_decoded, num_frames);

    // clean up allocated memory
    flexframegen_destroy(fg);
    flexframesync_destroy(fs);
    nco_crcf_destroy(nco_channel);
    interp_crcf_destroy(interp);
    free(payload);
    free(frame);

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
    if (verbose)
        printf("callback invoked\n");

    framedata * fd = (framedata*)_userdata;

    if (verbose) {
        printf("    header crc          : %s\n", _rx_header_valid ?  "pass" : "FAIL");
        printf("    payload length      : %u\n", _rx_payload_len);
        printf("    payload crc         : %s\n", _rx_payload_valid ?  "pass" : "FAIL");
        framesyncstats_print(&_stats);
    }
    if (!_rx_header_valid)
        return 0;

    // validate payload
    unsigned int i;
    unsigned int num_header_errors=0;
    for (i=0; i<14; i++)
        num_header_errors += (_rx_header[i] == fd->header[i]) ? 0 : 1;
    if (verbose)
        printf("    num header errors   : %u\n", num_header_errors);

    unsigned int num_payload_errors=0;
    for (i=0; i<_rx_payload_len; i++)
        num_payload_errors += (_rx_payload[i] == fd->payload[i]) ? 0 : 1;
    if (verbose)
        printf("    num payload errors  : %u\n", num_payload_errors);

    fd->num_frames_received++;

    if (num_payload_errors == 0)
        fd->num_payloads_decoded++;

    return 0;
}

