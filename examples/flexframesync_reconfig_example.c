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
static int callback(unsigned char *  _header,
                    int              _header_valid,
                    unsigned char *  _payload,
                    unsigned int     _payload_len,
                    int              _payload_valid,
                    framesyncstats_s _stats,
                    void *           _userdata);

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

    // create flexframesync object with default properties
    flexframesync fs = flexframesync_create(callback,(void*)&fd);

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

    // push through synchronizer
    float complex x[2];     // frame samples
    float complex y;        // 
    float complex noise;    //

    unsigned int j;
    for (j=0; j<num_frames; j++) {
        // configure frame generator properties
        unsigned int payload_len = (rand() % 256) + 1;   // random payload length
        fgprops.check            = LIQUID_CRC_NONE;      // data validity check
        fgprops.fec0             = LIQUID_FEC_NONE;      // inner FEC scheme
        fgprops.fec1             = LIQUID_FEC_NONE;      // outer FEC scheme
        fgprops.mod_scheme       = (rand() % 2) ? LIQUID_MODEM_QPSK : LIQUID_MODEM_QAM16;

        // set properties
        flexframegen_setprops(fg, &fgprops);
        if (verbose)
            flexframegen_print(fg);

        // reallocate memory for payload
        payload = realloc(payload, payload_len*sizeof(unsigned char));

        // initialize payload
        for (i=0; i<payload_len; i++)
            payload[i] = rand() & 0xff;

        // compute frame length
        frame_len = flexframegen_getframelen(fg);

        // reallocate memory for frame
        frame = realloc(frame, frame_len*sizeof(float complex));

        // set framedata pointers
        fd.header = header;
        fd.payload = payload;

        // assemble the frame
        flexframegen_assemble(fg, header, payload, payload_len);

        int frame_complete = 0;
        while (!frame_complete) {
            // compensate for filter delay
            frame_complete = flexframegen_write_samples(fg, x);

            for (i=0; i<2; i++) {
                // add channel impairments
                nco_crcf_mix_up(nco_channel, x[i], &y);
                nco_crcf_step(nco_channel);

                // apply channel gain
                y *= gamma;

                // add noise
                y += nstd * (randnf() + _Complex_I*randnf()) * M_SQRT1_2;

                // emulate sample timing offset with Farrow filter
                firfarrow_crcf_push(delay_filter, y);
                firfarrow_crcf_execute(delay_filter, &y);

                // push through sync
                flexframesync_execute(fs, &y, 1);
            }
        }

        // flush farrow filter with zeros
        for (i=0; i<200; i++) {
            // push noise through sync
            noise = nstd * (randnf() + _Complex_I*randnf()) * M_SQRT1_2;
            
            firfarrow_crcf_push(delay_filter, noise);
            firfarrow_crcf_execute(delay_filter, &y);

            flexframesync_execute(fs, &y, 1);
        }
    } // num frames

    printf("num frames received  : %3u / %3u\n", fd.num_frames_received,  num_frames);
    printf("num payloads decoded : %3u / %3u\n", fd.num_payloads_decoded, num_frames);

    // clean up allocated memory
    flexframegen_destroy(fg);
    flexframesync_destroy(fs);
    nco_crcf_destroy(nco_channel);
    free(payload);
    free(frame);

    printf("done.\n");
    return 0;
}

// flexframesync callback function
static int callback(unsigned char *  _header,
                    int              _header_valid,
                    unsigned char *  _payload,
                    unsigned int     _payload_len,
                    int              _payload_valid,
                    framesyncstats_s _stats,
                    void *           _userdata)
{
    if (verbose)
        printf("callback invoked\n");

    framedata * fd = (framedata*)_userdata;

    if (verbose) {
        printf("    header crc          : %s\n", _header_valid ?  "pass" : "FAIL");
        printf("    payload length      : %u\n", _payload_len);
        printf("    payload crc         : %s\n", _payload_valid ?  "pass" : "FAIL");
        framesyncstats_print(&_stats);
    }
    if (!_header_valid)
        return 0;

    // validate payload
    unsigned int i;
    unsigned int num_header_errors=0;
    for (i=0; i<14; i++)
        num_header_errors += (_header[i] == fd->header[i]) ? 0 : 1;
    if (verbose)
        printf("    num header errors   : %u\n", num_header_errors);

    unsigned int num_payload_errors=0;
    for (i=0; i<_payload_len; i++)
        num_payload_errors += (_payload[i] == fd->payload[i]) ? 0 : 1;
    if (verbose)
        printf("    num payload errors  : %u\n", num_payload_errors);

    fd->num_frames_received++;

    if (num_payload_errors == 0)
        fd->num_payloads_decoded++;

    return 0;
}

