//
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <getopt.h>

#include "liquid.h"

#define OUTPUT_FILENAME  "flexframesync_example.m"

static int callback(unsigned char * _rx_header,
                    int _rx_header_valid,
                    unsigned char * _rx_payload,
                    unsigned int _rx_payload_len,
                    void * _userdata);

typedef struct {
    unsigned char * header;
    unsigned char * payload;
    unsigned int num_frames_received;
} framedata;

void usage();

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
    while((dopt = getopt(argc,argv,"uhs:f:m:p:n:")) != EOF){
        switch (dopt) {
        case 'h':
        case 'u': usage(); return 0;
        case 's': SNRdB = atof(optarg); break;
        case 'f': packet_len = atol(optarg); break;
        case 'm':
            mod_scheme = liquid_getopt_str2mod(optarg);
            if (mod_scheme == MOD_UNKNOWN) {
                printf("error: unknown/unsupported mod. scheme: %s\n", optarg);
                usage();
                exit(-1);
            }
            break;
        case 'p': bps = atoi(optarg); break;
        case 'n': num_frames = atoi(optarg); break;
        default:
            printf("error: unknown option\n");
            usage();
            exit(-1);
        }
    }
    printf("blah\n");

    // create flexframegen object
    flexframegenprops_s fgprops;
    fgprops.rampup_len = 64;
    fgprops.phasing_len = 64;
    fgprops.payload_len = packet_len;
    fgprops.mod_scheme = mod_scheme;
    fgprops.mod_bps = bps;
    fgprops.rampdn_len = 64;
    flexframegen fg = flexframegen_create(&fgprops);
    flexframegen_print(fg);

    // frame data
    unsigned char header[8];
    unsigned char payload[fgprops.payload_len];
    framedata fd = {header, payload, 0};

    // create interpolator
    unsigned int h_len = 2*2*m + 1;
    float h[h_len];
    design_rrc_filter(2,m,beta,0,h);
    interp_crcf interp = interp_crcf_create(2,h,h_len);

    // create flexframesync object with default properties
    flexframesyncprops_s fsprops;
    flexframesyncprops_init_default(&fsprops);
    fsprops.squelch_threshold = noise_floor - 3.0f; // set threshold below noise floor
                                                    // to help ensure detection for weak
                                                    // signals
    fsprops.agc_bw0 = 1e-3f;
    fsprops.agc_bw1 = 1e-5f;
    fsprops.agc_gmin = 1e-3f;
    fsprops.agc_gmax = 1e4f;
    fsprops.pll_bw0 = 1e-3f;
    fsprops.pll_bw1 = 1e-4f;
    flexframesync fs = flexframesync_create(&fsprops,callback,(void*)&fd);
    flexframesync_print(fs);

    // channel
    float phi=0.0f;
    float dphi=0.02f;
    nco nco_channel = nco_create(LIQUID_VCO);
    nco_set_phase(nco_channel, phi);
    nco_set_frequency(nco_channel, dphi);
    float nstd  = powf(10.0f, noise_floor/10.0f);         // noise std. dev.
    float gamma = powf(10.0f, (SNRdB+noise_floor)/10.0f); // channel gain
    float mu    = 0.3f; // fractional sample delay
    fir_farrow_crcf delay_filter = fir_farrow_crcf_create(27,5,0.9f,60.0f);
    fir_farrow_crcf_set_delay(delay_filter,mu);

    unsigned int i;
    // initialize header, payload
    for (i=0; i<8; i++)
        header[i] = i;
    for (i=0; i<fgprops.payload_len; i++)
        payload[i] = rand() & 0xff;

    // internal test : encode/decode header
#if 0
    float complex header_modulated[128];
    flexframegen_encode_header(fg, header);
    flexframegen_modulate_header(fg, header_modulated);

    flexframesync_demodulate_header(fs, header_modulated);
    flexframesync_decode_header(fs, NULL);
#endif

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
        nco_mix_up(nco_channel, y[0], &z[0]);
        nco_step(nco_channel);
        nco_mix_up(nco_channel, y[1], &z[1]);
        nco_step(nco_channel);

        // apply channel gain
        z[0] *= gamma;
        z[1] *= gamma;

        // add noise
        cawgn(&z[0], nstd);
        cawgn(&z[1], nstd);

        // emulate sample timing offset with Farrow filter
        fir_farrow_crcf_push(delay_filter, z[0]);
        fir_farrow_crcf_execute(delay_filter, &z[0]);
        fir_farrow_crcf_push(delay_filter, z[1]);
        fir_farrow_crcf_execute(delay_filter, &z[1]);

        // push through sync
        flexframesync_execute(fs, z, 2);
    }
    // flush frame
    for (i=0; i<64; i++) {
        noise = 0.0f;
        cawgn(&noise, nstd);
        // push noise through sync
        interp_crcf_execute(interp, noise, y);
        fir_farrow_crcf_push(delay_filter, y[0]);
        fir_farrow_crcf_execute(delay_filter, &z[0]);
        fir_farrow_crcf_push(delay_filter, y[1]);
        fir_farrow_crcf_execute(delay_filter, &z[1]);
        flexframesync_execute(fs, z, 2);
    }
    } // num frames

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
#if 0
    float complex frame_rx[2048];
    
    // push noise
    for (i=0; i<2048; i++) {
        frame_rx[i] = (randnf() + _Complex_I*randnf())*0.01f*gamma;
    }
    flexframesync_execute(fs, frame_rx, 2048);

    flexframegen_execute(fg, header, payload, frame_rx);

    // add channel impairments
    for (i=0; i<2048; i++) {
        frame_rx[i] *= cexpf(_Complex_I*phi);
        frame_rx[i] += (randnf() + _Complex_I*randnf())*0.01f;
        frame_rx[i] *= gamma;
        nco_mix_up(nco_channel, frame_rx[i], &frame_rx[i]);

        nco_step(nco_channel);
    }

    // synchronize/receive the frame
    flexframesync_execute(fs, frame_rx, 2048);

    // write frame to output file
    FILE* fid = fopen(OUTPUT_FILENAME, "w");
    fprintf(fid,"%% %s: auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"\n\n");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"\n\n");
    for (i=0; i<2048; i++)
        fprintf(fid, "frame_rx(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(frame_rx[i]), cimagf(frame_rx[i]));

    fprintf(fid,"t=0:2047;\n");
    fprintf(fid,"plot(t,real(frame_rx),t,imag(frame_rx));\n");
    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);
#endif

    flexframegen_destroy(fg);
    flexframesync_destroy(fs);
    nco_destroy(nco_channel);
    interp_crcf_destroy(interp);

    printf("done.\n");
    return 0;
}

void usage()
{
    printf("  u/h   :   print usage\n");
    printf("  s     :   SNR [dB], 30\n");
    printf("  f     :   frame bytes (packet len), 64\n");
    printf("  m     :   mod scheme, [psk], dpsk, pam, qam\n");
    printf("  p     :   bits per symbol, 1\n");
    printf("  n     :   num frames, 3\n");
}

static int callback(unsigned char * _rx_header,
                    int _rx_header_valid,
                    unsigned char * _rx_payload,
                    unsigned int _rx_payload_len,
                    void * _userdata)
{
    printf("callback invoked\n");

    framedata * fd = (framedata*)_userdata;

    printf("    header crc          : %s\n", _rx_header_valid ?  "pass" : "FAIL");
    printf("    payload length      : %u\n", _rx_payload_len);
    if (!_rx_header_valid)
        return 0;

    // validate payload
    unsigned int i;
    unsigned int num_header_errors=0;
    for (i=0; i<8; i++)
        num_header_errors += (_rx_header[i] == fd->header[i]) ? 0 : 1;
    printf("    num header errors   : %u\n", num_header_errors);

    unsigned int num_payload_errors=0;
    for (i=0; i<_rx_payload_len; i++)
        num_payload_errors += (_rx_payload[i] == fd->payload[i]) ? 0 : 1;
    printf("    num payload errors  : %u\n", num_payload_errors);

    fd->num_frames_received++;

    return 0;
}

