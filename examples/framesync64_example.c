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
    printf("framesync64_example [options]\n");
    printf("  h     : print usage\n");
    printf("  d     : enable debugging\n");
    printf("  S     : signal-to-noise ratio [dB], default: 20\n");
    printf("  F     : carrier frequency offset, default: 0\n");
    printf("  P     : carrier phase offset, default: 0\n");
    printf("  T     : fractional sample timing offset, default: 0.01\n");
}

// static callback function
static int callback(unsigned char *  _header,
                    int              _header_valid,
                    unsigned char *  _payload,
                    unsigned int     _payload_len,
                    int              _payload_valid,
                    framesyncstats_s _stats,
                    void *           _userdata);

// global arrays
unsigned char header[8];
unsigned char payload[64];

int main(int argc, char*argv[])
{
    srand( time(NULL) );

    // options
    float SNRdB       =  20.0f; // signal-to-noise ratio
    float noise_floor = -40.0f; // noise floor
    float dphi        =  0.01f; // carrier frequency offset
    float theta       =  0.0f;  // carrier phase offset
    float dt          =  -0.2f;  // fractional sample timing offset
    int debug_enabled = 0;

    // get options
    int dopt;
    while((dopt = getopt(argc,argv,"hdS:F:P:T:")) != EOF){
        switch (dopt) {
        case 'h': usage();              return 0;
        case 'd': debug_enabled = 1;    break;
        case 'S': SNRdB = atof(optarg); break;
        case 'F': dphi  = atof(optarg); break;
        case 'P': theta = atof(optarg); break;
        case 'T': dt    = atof(optarg); break;
        default:
            exit(-1);
        }
    }

    // derived values
    unsigned int frame_len = LIQUID_FRAME64_LEN;          // fixed frame length
    unsigned int num_samples = frame_len + 200;           // total number of samples
    float nstd  = powf(10.0f, noise_floor/20.0f);         // noise std. dev.
    float gamma = powf(10.0f, (SNRdB+noise_floor)/20.0f); // channel gain

    // create frame generator
    framegen64 fg = framegen64_create();
    framegen64_print(fg);

    // create frame synchronizer using default properties
    framesync64 fs = framesync64_create(callback,NULL);
    framesync64_print(fs);
    if (debug_enabled)
        framesync64_debug_enable(fs);

    // data payload
    unsigned int i;
    // initialize header and payload data
    for (i=0; i<8; i++)
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
    firfilt_crcf finterp = firfilt_crcf_create_kaiser(2*d+1, 0.45f, 40.0f, -dt);
    for (i=0; i<num_samples; i++) {
        // fractional sample timing offset
        if      (i < 100)             firfilt_crcf_push(finterp, 0.0f);
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

    // synchronize/receive the frame
    framesync64_execute(fs, y, num_samples);

    // export debugging file
    if (debug_enabled)
        framesync64_debug_print(fs, "framesync64_debug.m");

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
                    unsigned int     _payload_len,
                    int              _payload_valid,
                    framesyncstats_s _stats,
                    void *           _userdata)
{
    printf("*** callback invoked ***\n");
    printf("    error vector mag.   : %12.8f dB\n", _stats.evm);
    printf("    rssi                : %12.8f dB\n", _stats.rssi);
    printf("    carrier offset      : %12.8f\n", _stats.cfo);
    printf("    mod. scheme         : %s\n", modulation_types[_stats.mod_scheme].fullname);
    printf("    mod. depth          : %u\n", _stats.mod_bps);
    printf("    payload CRC         : %s\n", crc_scheme_str[_stats.check][1]);
    printf("    payload fec (inner) : %s\n", fec_scheme_str[_stats.fec0][1]);
    printf("    payload fec (outer) : %s\n", fec_scheme_str[_stats.fec1][1]);
    printf("    header crc          : %s\n", _header_valid ? "pass" : "FAIL");
    printf("    header data         : %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n",
            _header[0], _header[1], _header[2], _header[3],
            _header[4], _header[5], _header[6], _header[7]);
    printf("    num header errors   : %u / %u\n",
            count_bit_errors_array(_header, header, 8),
            8*8);
    printf("    payload crc         : %s\n", _payload_valid ? "pass" : "FAIL");
    printf("    num payload errors  : %u / %u\n",
            count_bit_errors_array(_payload, payload, 64),
            64*8);

    return 0;
}

