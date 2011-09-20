//
// gmskframesync_example.c [EXPERIMENTAL]
//
// Example demonstrating the GMSK flexible frame synchronizer.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include <assert.h>

#include "liquid.h"
#include "liquid.experimental.h"

#define OUTPUT_FILENAME "gmskframesync_example.m"

void usage()
{
    printf("gmskframesync_example [options]\n");
    printf("  u/h   : print usage\n");
    printf("  n     : frame length [bytes], default: 40\n");
    printf("  s     : signal-to-noise ratio [dB], default: 30\n");
}

struct framedata_s {
    unsigned char * payload;
    unsigned int payload_len;
};

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
    unsigned int payload_len = 40;  // length of payload (bytes)
    float noise_floor = -60.0f;     // noise floor
    float SNRdB = 30.0f;            // signal-to-noise ratio [dB]
    float dphi  = 0.05f;            // carrier offset

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

    // validate options
    if (k < 2) {
        fprintf(stderr,"error: %s, samples per symbol must be at least 2\n", argv[0]);
        exit(1);
    } else if (m == 0) {
        fprintf(stderr,"error: %s, filter semi-length must be at least 1\n", argv[0]);
        exit(1);
    } else if (BT < 0.0f || BT > 1.0f) {
        fprintf(stderr,"error: %s, filter excess bandwidth must be in [0,1]\n", argv[0]);
        exit(1);
    }

    // derived values
    float nstd = powf(10.0f, noise_floor/20.0f);
    float gamma = powf(10.0f, (SNRdB + noise_floor)/20.0f);

    // allocate memory for payload and initialize
    unsigned char payload[payload_len];
    for (i=0; i<payload_len; i++)
        payload[i] = rand() & 0xff;
    struct framedata_s fd = {payload, payload_len};

    // create frame generator
    gmskframegen fg = gmskframegen_create(k, m, BT);
    gmskframegen_print(fg);

    // create frame synchronizer
    gmskframesync fs = gmskframesync_create(k, m, BT, callback, (void*)&fd);
    gmskframesync_print(fs);

    // assemble frame
    gmskframegen_assemble(fg, payload, payload_len);

    // allocate memory for full frame
    unsigned int frame_len = gmskframegen_get_frame_len(fg);
    unsigned int num_samples = frame_len + 800;
    float complex x[num_samples];
    float complex y[num_samples];

    // generate frame
    unsigned int n=0;
    for (i=0; i<600; i++) x[n++] = 0.0f;
    int frame_complete = 0;
    while (!frame_complete) {
        frame_complete = gmskframegen_write_samples(fg, &x[n]);
        n += k;
    }
    for (i=0; i<200; i++) x[n++] = 0.0f;
    assert(n==num_samples);

    // add channel impairments
    for (i=0; i<num_samples; i++) {
        y[i]  = gamma * x[i] * cexpf(_Complex_I*dphi*i);
        y[i] += nstd*(randnf() + randnf()*_Complex_I)*M_SQRT1_2;
    }

    // push samples through synchronizer
    gmskframesync_execute(fs, y, num_samples);

    // destroy objects
    gmskframegen_destroy(fg);
    gmskframesync_destroy(fs);


    // 
    // export output
    //
    FILE * fid = fopen(OUTPUT_FILENAME,"w");
    if (fid == NULL) {
        fprintf(stderr,"error: %s, could not open '%s' for writing\n", argv[0], OUTPUT_FILENAME);
        exit(1);
    }
    fprintf(fid,"%% %s : auto-generated file\n", OUTPUT_FILENAME);
    fprintf(fid,"\n");
    fprintf(fid,"clear all\n");
    fprintf(fid,"close all\n");
    fprintf(fid,"\n");
    fprintf(fid,"num_samples = %u;\n", num_samples);
    fprintf(fid,"y = zeros(1,num_samples);\n");
    fprintf(fid,"\n");

    for (i=0; i<num_samples; i++)
        fprintf(fid,"y(%6u) = %12.4e + j*%12.4e;\n", i+1, crealf(y[i]), cimagf(y[i]));

    fprintf(fid,"\n");
    fprintf(fid,"t = 0:(num_samples-1);\n");
    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(t, real(y), t,imag(y));\n");
    fprintf(fid,"xlabel('time');\n");
    fprintf(fid,"ylabel('received signal');\n");
    fprintf(fid,"legend('real','imag',0);\n");
    fclose(fid);
    printf("results written to '%s'\n", OUTPUT_FILENAME);

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
    printf("**** callback invoked : rssi = %8.3f dB, evm = %8.3f dB\n", _stats.rssi, _stats.evm);

    // count errors
    struct framedata_s * fd = (struct framedata_s *) _userdata;
    unsigned int bit_errors = count_bit_errors_array(fd->payload, _payload, _payload_len);
    printf("    bit errors : %4u / %4u\n", bit_errors, 8*_payload_len);

    return 0;
}

