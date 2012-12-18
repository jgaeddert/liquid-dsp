//
// framesync64_example.c
//
// This example demonstrates the interfaces to the framegen64 and
// framesync64 objects used to completely encapsulate data for
// over-the-air transmission.  A 64-byte payload is generated, and then
// encoded, modulated, and interpolated using the framegen64 object.
// The resulting complex baseband samples are corrupted with noise and
// moderate carrier frequency and phase offsets before the framesync64
// object attempts to decode the frame.  The resulting data are compared
// to the original to validate correctness.
//
// SEE ALSO: flexframesync_example.c
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <getopt.h>
#include <time.h>

#include "liquid.h"

#define OUTPUT_FILENAME  "framesync64_example.m"

void usage()
{
    printf("ofdmflexframesync_example [options]\n");
    printf("  h     : print usage\n");
    printf("  S     : signal-to-noise ratio [dB], default: 30\n");
    printf("  F     : carrier frequency offset, default: 0\n");
    printf("  P     : carrier phase offset, default: 0\n");
    printf("  T     : fractional sample timing offset, default: 0\n");
}

// static callback function
static int callback(unsigned char *  _header,
                    int              _header_valid,
                    unsigned char *  _payload,
                    int              _payload_valid,
                    framesyncstats_s _stats,
                    void *           _userdata);

static void callback_csma_lock(void * _userdata);
static void callback_csma_unlock(void * _userdata);

// global header, payload arrays
unsigned char header[12];
unsigned char payload[64];

int main(int argc, char*argv[])
{
    srand( time(NULL) );

    // options
    float SNRdB       =  20.0f; // signal-to-noise ratio
    float noise_floor = -40.0f; // noise floor
    float dphi        =  0.0f;  // carrier frequency offset
    float theta       =  0.0f;  // carrier phase offset
    float dt          =  0.0f;  // fractional sample timing offset

    // create framegen64 object
    unsigned int m=3;
    float beta=0.7f;
    
    // get options
    int dopt;
    while((dopt = getopt(argc,argv,"hS:F:P:T:")) != EOF){
        switch (dopt) {
        case 'h': usage();              return 0;
        case 'S': SNRdB = atof(optarg); break;
        case 'F': dphi  = atof(optarg); break;
        case 'P': theta = atof(optarg); break;
        case 'T': dt    = atof(optarg); break;
        default:
            exit(-1);
        }
    }

    // channel
    unsigned int frame_len = 1244;              // fixed frame length
    unsigned int num_samples = frame_len + 200; // total number of samples
    float nstd  = powf(10.0f, noise_floor/20.0f);         // noise std. dev.
    float gamma = powf(10.0f, (SNRdB+noise_floor)/20.0f); // channel gain

    // create frame generator
    framegen64 fg = framegen64_create(m,beta);

    // create frame synchronizer using default properties
    framesync64 fs = framesync64_create(NULL,callback,NULL);

    // set advanced csma callback functions
    framesync64_set_csma_callbacks(fs, callback_csma_lock, callback_csma_unlock, NULL);
    framesync64_print(fs);

    // data payload
    unsigned int i;
    // initialize header, payload
    for (i=0; i<12; i++)
        header[i] = i;
    for (i=0; i<64; i++)
        payload[i] = rand() & 0xff;

    // allocate memory for the frame samples
    float complex frame[frame_len]; // generated frame
    float complex y[num_samples];   // received sequence
    
    // generate the frame
    framegen64_execute(fg, header, payload, frame);

    // fractional sample timing offset
    unsigned int d = 11;    // fractional sample filter delay
    firfilt_crcf finterp = firfilt_crcf_create_kaiser(2*d+1, 0.45f, 40.0f, dt);
    for (i=0; i<num_samples; i++) {
        // fractional sample timing offset
        if (i < 100)                  firfilt_crcf_push(finterp, 0.0f);
        else if (i < frame_len + 100) firfilt_crcf_push(finterp, frame[i-100]);
        else                          firfilt_crcf_push(finterp, 0.0f);

        // compute output
        firfilt_crcf_execute(finterp, &y[i]);
    }
    firfilt_crcf_destroy(finterp);

    // add channel impairments
    for (i=0; i<num_samples; i++) {
        y[i] *= cexpf(_Complex_I*(dphi*i +theta));
        y[i] *= gamma;
        y[i] += nstd*( randnf() + _Complex_I*randnf())*M_SQRT1_2;
    }

#if 0
    // push noise through synchronizer
    for (i=0; i<300; i++) {
        float complex noise = nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;
        framesync64_execute(fs, &noise, 1);
    }
#endif

    // synchronize/receive the frame
    framesync64_execute(fs, y, num_samples);

    // clean up allocated objects
    framegen64_destroy(fg);
    framesync64_destroy(fs);

    // 
    // export results
    //
    FILE* fid = fopen(OUTPUT_FILENAME, "w");
    fprintf(fid,"%% %s: auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"\n\n");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    fprintf(fid,"frame_len   = %u;\n", frame_len);
    fprintf(fid,"num_samples = %u;\n", num_samples);
    for (i=0; i<num_samples; i++)
        fprintf(fid, "y(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));

    fprintf(fid,"t=0:(length(y)-1);\n");
    fprintf(fid,"plot(t,real(y),t,imag(y));\n");
    fclose(fid);
    printf("results written to %s\n", OUTPUT_FILENAME);

    printf("done.\n");
    return 0;
}

// static callback function
static int callback(unsigned char *  _header,
                    int              _header_valid,
                    unsigned char *  _payload,
                    int              _payload_valid,
                    framesyncstats_s _stats,
                    void *           _userdata)
{
    printf("*** callback invoked ***\n");
    printf("    error vector mag.   : %12.8f dB\n", _stats.evm);
    printf("    rssi                : %12.8f dB\n", _stats.rssi);
    printf("    mod. scheme         : %s\n", modulation_types[_stats.mod_scheme].fullname);
    printf("    mod. depth          : %u\n", _stats.mod_bps);
    printf("    payload CRC         : %s\n", crc_scheme_str[_stats.check][1]);
    printf("    payload fec (inner) : %s\n", fec_scheme_str[_stats.fec0][1]);
    printf("    payload fec (outer) : %s\n", fec_scheme_str[_stats.fec1][1]);

    printf("    header crc          : %s\n", _header_valid ?  "pass" : "FAIL");
    printf("    payload crc         : %s\n", _payload_valid ? "pass" : "FAIL");

    // validate payload
    unsigned int i;
    unsigned int num_header_errors=0;
    for (i=0; i<12; i++)
        num_header_errors += (_header[i] == header[i]) ? 0 : 1;
    printf("    num header errors   : %u\n", num_header_errors);

    unsigned int num_payload_errors=0;
    for (i=0; i<64; i++)
        num_payload_errors += (_payload[i] == payload[i]) ? 0 : 1;
    printf("    num payload errors  : %u\n", num_payload_errors);

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

