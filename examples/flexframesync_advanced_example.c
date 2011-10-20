//
// flexframesync_advanced_example.c
//
// This example demonstrates the interfaces to the flexframegen and
// flexframesync objects used to completely encapsulate raw data bytes
// into frame samples (nearly) ready for over-the-air transmission. A
// 14-byte header and variable length payload are encoded into baseband
// symbols using the flexframegen object.  The resulting symbols are
// interpolated using a root-Nyquist filter before adding channel
// impairments (noise, carrier frequency/phase offset, timing phase
// offset, etc).  The resulting samples are then fed into the
// flexframesync object which attempts to decode the frames.  Whenever a
// frame is found and properly decoded, its callback function is
// invoked.
//
// SEE ALSO: flexframesync_example.c
//           flexframesync_reconfig_example.c
//           framesync64_example.c
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <getopt.h>

#include "liquid.h"

#define OUTPUT_FILENAME  "flexframesync_advaned_example.m"

void usage()
{
    printf("flexframesync_example [options]\n");
    printf("  u/h   : print usage\n");
    printf("  v/q   : verbose/quiet output\n");
    printf("  s     : signal-to-noise ratio [dB], default: 30\n");
    printf("  c     : multi-path channel taps, default: 0\n");
    printf("  e     : equalizer taps, default: 0\n");
    printf("  f     : frame length [bytes], default: 64\n");
    printf("  n     : number of frames, default: 3\n");
    printf("  m     : modulation scheme (qpsk default)\n");
    liquid_print_modulation_schemes();
}

// flexframesync callback function
static int callback(unsigned char * _rx_header,
                    int _rx_header_valid,
                    unsigned char * _rx_payload,
                    unsigned int _rx_payload_len,
                    int _rx_payload_valid,
                    framesyncstats_s _stats,
                    void * _userdata);

static void callback_csma_lock(void * _userdata);
static void callback_csma_unlock(void * _userdata);

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
    modulation_scheme mod_scheme = LIQUID_MODEM_QPSK;
    unsigned int bps = 2;
    unsigned int payload_len = 64;
    unsigned int num_frames = 3;
    unsigned int hc_len = 1;        // multi-path channel taps
    unsigned int eq_len = 0;        // equalizer taps

    // get options
    int dopt;
    while((dopt = getopt(argc,argv,"uhvqs:c:e:f:m:n:")) != EOF){
        switch (dopt) {
        case 'u':
        case 'h': usage();                      return 0;
        case 'v': verbose=1;                    break;
        case 'q': verbose=0;                    break;
        case 's': SNRdB = atof(optarg);         break;
        case 'c': hc_len = atoi(optarg)+1;      break;
        case 'e': eq_len = atoi(optarg);        break;
        case 'f': payload_len = atol(optarg);   break;
        case 'n': num_frames = atoi(optarg);    break;
        case 'm':
            liquid_getopt_str2modbps(optarg, &mod_scheme, &bps);
            if (mod_scheme == LIQUID_MODEM_UNKNOWN) {
                fprintf(stderr,"error: %s, unknown/unsupported mod. scheme: %s\n", argv[0], optarg);
                exit(-1);
            }
            break;
        default:
            exit(1);
        }
    }

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
    fgprops.mod_bps     = bps;
    fgprops.rampdn_len  = 64;
    flexframegen fg = flexframegen_create(&fgprops);
    if (verbose)
        flexframegen_print(fg);

    // frame data
    unsigned char header[14];
    unsigned char payload[fgprops.payload_len];
    framedata fd = {header, payload, 0};

    // create interpolator
    interp_crcf interp = interp_crcf_create_rnyquist(LIQUID_RNYQUIST_RRC,2,m,beta,0);

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
    fsprops.eq_len = eq_len;        // number of equalizer taps
    //fsprops.eqrls_lambda = 0.999f;  // RLS equalizer forgetting factor, 0.999 typical
    flexframesync fs = flexframesync_create(&fsprops,callback,(void*)&fd);

    // set advanced csma callback functions
    flexframesync_set_csma_callbacks(fs, callback_csma_lock, callback_csma_unlock, NULL);

    if (verbose)
        flexframesync_print(fs);

    // channel
    float phi=2.0f;
    float dphi=0.02f;
    nco_crcf nco_channel = nco_crcf_create(LIQUID_VCO);
    nco_crcf_set_phase(nco_channel, phi);
    nco_crcf_set_frequency(nco_channel, dphi);
    float nstd  = powf(10.0f, noise_floor/20.0f);         // noise std. dev.
    float gamma = powf(10.0f, (SNRdB+noise_floor)/20.0f); // channel gain
    float mu    = 0.3f; // fractional sample delay
    firfarrow_crcf delay_filter = firfarrow_crcf_create(27,5,0.45f,60.0f);
    firfarrow_crcf_set_delay(delay_filter,mu);

    // multi-path channel
    unsigned int i;
#if 0
    // fixed equalizer taps
    hc_len = 6;
    float complex hc[] = {
          1.0000e+00 + _Complex_I*  0.0000e+00,
         -4.0412e-01 + _Complex_I*  2.0199e-01,
          1.2094e-01 + _Complex_I* -3.8529e-01,
         -9.7195e-02 + _Complex_I* -9.8874e-02,
         -1.3591e-01 + _Complex_I*  6.5303e-02,
         -3.1153e-02 + _Complex_I* -8.8593e-02};
#else
    // random equalizer taps
    float complex hc[hc_len];
    hc[0] = 1.0f;
    for (i=1; i<hc_len; i++)
        hc[i] = (randnf() + randnf()*_Complex_I) * 0.2f;
#endif
    firfilt_cccf fchannel = firfilt_cccf_create(hc,hc_len);
    for (i=0; i<hc_len; i++)
        printf("hc(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(hc[i]), cimagf(hc[i]));

    // initialize header, payload
    for (i=0; i<14; i++)
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

        // push through multi-path channel
        firfilt_cccf_push(fchannel, z[0]);
        firfilt_cccf_execute(fchannel, &z[0]);
        firfilt_cccf_push(fchannel, z[1]);
        firfilt_cccf_execute(fchannel, &z[1]);

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
    firfilt_cccf_destroy(fchannel);

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

    return 0;
}

static void callback_csma_lock(void * _userdata)
{
    printf("*** SIGNAL HIGH ***\n");
}

static void callback_csma_unlock(void * _userdata)
{
    printf("*** SIGNAL LOW ***\n");
}

