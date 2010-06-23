//
// flexframesync_example.c
//
// This example demonstrates the interfaces to the flexframegen and
// flexframesync objects used to completely encapsulate raw data bytes
// into frame samples (nearly) ready for over-the-air transmission. An
// 9-byte header and variable length payload are encoded into baseband
// symbols using the flexframegen object.  The resulting symbols are
// interpolated using a root-Nyquist filter before adding channel
// impairments (noise, carrier frequency/phase offset, timing phase
// offset, etc).  The resulting samples are then fed into the
// flexframesync object which attempts to decode the frames.  Whenever a
// frame is found and properly decoded, its callback function is
// invoked.
//
// SEE ALSO: flexframesync_reconf_example.c
//           framesync64_example.c
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <getopt.h>

#include "liquid.h"

#define OUTPUT_FILENAME  "flexframesync_example.m"

// output frame symbols to file?
#define OUTPUT_SYMBOLS_FILE 0

void usage()
{
    printf("flexframesync_example [options]\n");
    printf("  u/h   : print usage\n");
    printf("  v/q   : verbose/quiet output\n");
    printf("  s     : signal-to-noise ratio [dB], default: 30\n");
    printf("  f     : frame length [bytes], default: 64\n");
    printf("  n     : number of frames, default: 3\n");
    printf("  p     : modulation depth (default 2 bits/symbol)\n");
    printf("  m     : modulation scheme (psk default)\n");
    // print all available MOD schemes
    unsigned int i;
    for (i=0; i<LIQUID_NUM_MOD_SCHEMES; i++)
        printf("          %s\n", modulation_scheme_str[i]);
}

// flexframesync callback function
static int callback(unsigned char * _rx_header,
                    int _rx_header_valid,
                    unsigned char * _rx_payload,
                    unsigned int _rx_payload_len,
                    void * _userdata,
                    float complex * _frame_samples,
                    unsigned int _frame_samples_len);

// framedata object definition
typedef struct {
    unsigned char * header;
    unsigned char * payload;
    unsigned int num_frames_received;
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
    modulation_scheme mod_scheme = MOD_PSK;
    unsigned int bps = 1;
    unsigned int packet_len = 64;
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
        case 'f': packet_len = atol(optarg);    break;
        case 'n': num_frames = atoi(optarg);    break;
        case 'p': bps = atoi(optarg);           break;
        case 'm':
            mod_scheme = liquid_getopt_str2mod(optarg);
            if (mod_scheme == MOD_UNKNOWN) {
                printf("error: unknown/unsupported mod. scheme: %s\n", optarg);
                usage();
                exit(-1);
            }
            break;
        default:
            printf("error: unknown option\n");
            usage();
            exit(-1);
        }
    }

    // create flexframegen object
    flexframegenprops_s fgprops;
    fgprops.rampup_len = 64;
    fgprops.phasing_len = 64;
    fgprops.payload_len = packet_len;
    fgprops.mod_scheme = mod_scheme;
    fgprops.mod_bps = bps;
    fgprops.rampdn_len = 64;
    flexframegen fg = flexframegen_create(&fgprops);
    if (verbose)
        flexframegen_print(fg);

    // frame data
    unsigned char header[9];
    unsigned char payload[fgprops.payload_len];
    framedata fd = {header, payload, 0};

    // create interpolator
    unsigned int h_len = 2*2*m + 1;
    float h[h_len];
    design_rrc_filter(2,m,beta,0,h);
    interp_crcf interp = interp_crcf_create(2,h,h_len);

    // create flexframesync object with default properties
    framesyncprops_s fsprops;
    framesyncprops_init_default(&fsprops);
    fsprops.squelch_threshold = noise_floor + 3.0f;
    //fsprops.agc_bw0 = 1e-3f;
    //fsprops.agc_bw1 = 1e-5f;
    //fsprops.agc_gmin = 1e-3f;
    //fsprops.agc_gmax = 1e4f;
    //fsprops.pll_bw0 = 0.020f;
    //fsprops.pll_bw1 = 0.005f;
    flexframesync fs = flexframesync_create(&fsprops,callback,(void*)&fd);
    if (verbose)
        flexframesync_print(fs);

    // channel
    float phi=0.0f;
    float dphi=0.02f;
    nco_crcf nco_channel = nco_crcf_create(LIQUID_VCO);
    nco_crcf_set_phase(nco_channel, phi);
    nco_crcf_set_frequency(nco_channel, dphi);
    float nstd  = powf(10.0f, noise_floor/10.0f);         // noise std. dev.
    float gamma = powf(10.0f, (SNRdB+noise_floor)/10.0f); // channel gain
    float mu    = 0.3f; // fractional sample delay
    firfarrow_crcf delay_filter = firfarrow_crcf_create(27,5,0.9f,60.0f);
    firfarrow_crcf_set_delay(delay_filter,mu);

    unsigned int i;
    // initialize header, payload
    for (i=0; i<9; i++)
        header[i] = i;
    for (i=0; i<fgprops.payload_len; i++)
        payload[i] = rand() & 0xff;

    // generate the frame
    unsigned int frame_len = flexframegen_getframelen(fg);

    float complex frame[frame_len];
    flexframegen_execute(fg, header, payload, frame);

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
    for (i=0; i<64; i++) {
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

    // destroy allocated objects
    flexframegen_destroy(fg);
    flexframesync_destroy(fs);
    nco_crcf_destroy(nco_channel);
    interp_crcf_destroy(interp);

    printf("num frames received : %3u / %3u\n", fd.num_frames_received, num_frames);

    // write to file
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    for (i=0; i<frame_len; i++)
        fprintf(fid,"frame(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(frame[i]), cimagf(frame[i]));
    fprintf(fid,"frame_len=%u;\n", frame_len);
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(1:frame_len,real(frame),1:frame_len,imag(frame));\n");
    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

static int callback(unsigned char * _rx_header,
                    int _rx_header_valid,
                    unsigned char * _rx_payload,
                    unsigned int _rx_payload_len,
                    void * _userdata,
                    float complex * _frame_samples,
                    unsigned int _frame_samples_len)
{
    if (verbose)
        printf("callback invoked\n");

    framedata * fd = (framedata*)_userdata;

    if (verbose) {
        printf("    header crc          : %s\n", _rx_header_valid ?  "pass" : "FAIL");
        printf("    payload length      : %u\n", _rx_payload_len);
    }
    if (!_rx_header_valid)
        return 0;

    // validate payload
    unsigned int i;
    unsigned int num_header_errors=0;
    for (i=0; i<9; i++)
        num_header_errors += (_rx_header[i] == fd->header[i]) ? 0 : 1;
    if (verbose)
        printf("    num header errors   : %u\n", num_header_errors);

    unsigned int num_payload_errors=0;
    for (i=0; i<_rx_payload_len; i++)
        num_payload_errors += (_rx_payload[i] == fd->payload[i]) ? 0 : 1;
    if (verbose)
        printf("    num payload errors  : %u\n", num_payload_errors);

    fd->num_frames_received++;

    // print frame_samples to output file
#if OUTPUT_SYMBOLS_FILE == 1
    FILE * fid = fopen("frame_samples.m","w");
    fprintf(fid,"clear all; close all;\n");
    for (i=0; i<_frame_samples_len; i++)
        fprintf(fid,"s(%6u) = %16.8e + j*%16.8e;\n", i+1,
                                                     crealf(_frame_samples[i]),
                                                     cimagf(_frame_samples[i]));
    fprintf(fid,"plot(real(s),imag(s),'x');\n");
    fprintf(fid,"axis([-1 1 -1 1]*1.5);\n");
    fprintf(fid,"axis square;\n");
    fclose(fid);
    if (verbose)
        printf("frame syms written to frame_samples.m\n");
#endif

    return 0;
}

