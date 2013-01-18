//
// gmskframesync_example.c
//
// Example demonstrating the GMSK flexible frame synchronizer.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <getopt.h>
#include <time.h>

#include "liquid.h"

#define OUTPUT_FILENAME "gmskframesync_example.m"

void usage()
{
    printf("gmskframesync_example [options]\n");
    printf("  h     : print help\n");
    printf("  d     : enable internal synchronizer debugging\n");
    printf("  n     : frame length [bytes], default: 40\n");
    printf("  v     : data integrity check: crc32 default\n");
    liquid_print_crc_schemes();
    printf("  c     : coding scheme (inner): h74 default\n");
    printf("  k     : coding scheme (outer): none default\n");
    liquid_print_fec_schemes();
    printf("  s     : signal-to-noise ratio [dB], default: 30\n");
    printf("  F     : carrier frequency offset, default: 0.05\n");
}

struct framedata_s {
    unsigned char * payload;
    unsigned int payload_len;
};

// callback function
int callback(unsigned char *  _header,
             int              _header_valid,
             unsigned char *  _payload,
             unsigned int     _payload_len,
             int              _payload_valid,
             framesyncstats_s _stats,
             void *           _userdata);

int main(int argc, char*argv[])
{
    srand(time(NULL));

    unsigned int payload_len = 40;  // length of payload (bytes)
    crc_scheme check = LIQUID_CRC_32;
    fec_scheme fec0  = LIQUID_FEC_HAMMING128;
    fec_scheme fec1  = LIQUID_FEC_NONE;
    float noise_floor = -60.0f;     // noise floor
    float SNRdB = 30.0f;            // signal-to-noise ratio [dB]
    float dphi  = 0.05f;            // carrier offset

    int debug_enabled = 0;          // debug option

    // get options
    int dopt;
    while((dopt = getopt(argc,argv,"hdn:v:c:k:s:F:")) != EOF){
        switch (dopt) {
        case 'u':
        case 'h': usage();                      return 0;
        case 'd': debug_enabled = 1;            break;
        case 'n': payload_len   = atoi(optarg); break;
        case 'v':
            // data integrity check
            check = liquid_getopt_str2crc(optarg);
            if (check == LIQUID_CRC_UNKNOWN) {
                fprintf(stderr,"error: unknown/unsupported CRC scheme \"%s\"\n\n",optarg);
                exit(1);
            }
            break;
        case 'c':
            // inner FEC scheme
            fec0 = liquid_getopt_str2fec(optarg);
            if (fec0 == LIQUID_FEC_UNKNOWN) {
                fprintf(stderr,"error: unknown/unsupported inner FEC scheme \"%s\"\n\n",optarg);
                exit(1);
            }
            break;
        case 'k':
            // outer FEC scheme
            fec1 = liquid_getopt_str2fec(optarg);
            if (fec1 == LIQUID_FEC_UNKNOWN) {
                fprintf(stderr,"error: unknown/unsupported outer FEC scheme \"%s\"\n\n",optarg);
                exit(1);
            }
            break;
        case 's': SNRdB         = atof(optarg); break;
        case 'F': dphi          = atof(optarg); break;
        default:
            exit(1);
        }
    }

    unsigned int i;

    // fixed values
    unsigned int k = 2;

    // derived values
    float nstd  = powf(10.0f, noise_floor/20.0f);
    float gamma = powf(10.0f, (SNRdB + noise_floor)/20.0f);

    // allocate memory for payload and initialize
    unsigned char header[8];
    unsigned char payload[payload_len];
    for (i=0; i<8; i++)           header[i]  = i;
    for (i=0; i<payload_len; i++) payload[i] = rand() & 0xff;
    struct framedata_s fd = {payload, payload_len};

    // create frame generator
    gmskframegen fg = gmskframegen_create();

    // create frame synchronizer
    gmskframesync fs = gmskframesync_create(callback, (void*)&fd);
    if (debug_enabled)
        gmskframesync_debug_enable(fs);

    // assemble frame and print
    gmskframegen_assemble(fg, header, payload, payload_len, check, fec0, fec1);
    gmskframegen_print(fg);

    // allocate memory for full frame (with noise)
    unsigned int frame_len = gmskframegen_getframelen(fg);
    unsigned int num_samples = (frame_len * k) + 800;
    float complex x[num_samples];
    float complex y[num_samples];

    // 
    // generate frame
    //
    unsigned int n=0;
    for (n=0; n<600; n++)
        x[n] = 0.0f;
    int frame_complete = 0;
    while (!frame_complete) {
        frame_complete = gmskframegen_write_samples(fg, &x[n]);
        n += k;
    }
    for ( ; n<num_samples; n++)
        x[n] = 0.0f;

    // add channel impairments
    for (i=0; i<num_samples; i++) {
        y[i]  = x[i];
        y[i] *= gamma;
        y[i] *= cexpf(_Complex_I*M_2_PI*dphi*i);
        y[i] += nstd*(randnf() + randnf()*_Complex_I)*M_SQRT1_2;
    }

    // push samples through synchronizer
    gmskframesync_execute(fs, y, num_samples);

    // write debug output if enabled
    if (debug_enabled)
        gmskframesync_debug_print(fs, "gmskframesync_debug.m");

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
int callback(unsigned char *  _header,
             int              _header_valid,
             unsigned char *  _payload,
             unsigned int     _payload_len,
             int              _payload_valid,
             framesyncstats_s _stats,
             void *           _userdata)
{
    printf("***** callback invoked *****\n");
    printf("  header crc    :   %s\n", _header_valid ? "pass" : "FAIL");
    printf("  header data   :  ");
    unsigned int i;
    for (i=0; i<8; i++)
        printf(" %.2X", _header[i]);
    printf("\n");
    printf("  rssi          :   %-8.3f dB\n", _stats.rssi);
    printf("  evm           :   %-8.3f dB\n", _stats.evm);
    printf("  payload       :   %u bytes (crc %s)\n", _payload_len, _payload_valid ? "pass" : "FAIL");
    printf("  check         :   %s\n", crc_scheme_str[_stats.check][1]);
    printf("  fec (inner)   :   %s\n", fec_scheme_str[_stats.fec0][1]);
    printf("  fec (outer)   :   %s\n", fec_scheme_str[_stats.fec1][1]);

    // count errors
    struct framedata_s * fd = (struct framedata_s *) _userdata;
    unsigned int bit_errors = count_bit_errors_array(fd->payload, _payload, _payload_len);
    printf("  bit errors    :   %-4u / %-4u\n", bit_errors, 8*_payload_len);

    return 0;
}

