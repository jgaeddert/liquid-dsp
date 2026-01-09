char __docstr__[] = "Demonstrate the interface to dsssframesync";

#include <assert.h>
#include <getopt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "liquid.h"
#include "liquid.argparse.h"

// dsssframesync callback function
static int callback(const unsigned char *  _header,
                    int                    _header_valid,
                    const unsigned char *  _payload,
                    unsigned int           _payload_len,
                    int                    _payload_valid,
                    framesyncstats_s       _stats,
                    void *                 _userdata);

int main(int argc, char * argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*,    crc_type,      "crc32", 'v', "data integrity check", liquid_argparse_crc);
    liquid_argparse_add(char*,    fec0_type,     "none",  'c', "inner code", liquid_argparse_fec);
    liquid_argparse_add(char*,    fec1_type,     "none",  'k', "outer code", liquid_argparse_fec);
    liquid_argparse_add(unsigned, payload_len,   20,      'n', "payload length", NULL);
    liquid_argparse_add(float,    noise_floor,   -60.0f,  '0', "noise floor", NULL);
    liquid_argparse_add(float,    SNRdB,         -3.0f,   's', "signal-to-noise ratio", NULL);
    liquid_argparse_add(float,    dphi,          0.005f,  'F', "carrier frequency offset", NULL);
    liquid_argparse_add(int,      debug_enabled, 0,       'd', "enable debugging", NULL);
    liquid_argparse_parse(argc,argv);

    // validate options
    crc_scheme check= liquid_getopt_str2crc(crc_type);
    fec_scheme fec0 = liquid_getopt_str2fec(fec0_type);
    fec_scheme fec1 = liquid_getopt_str2fec(fec1_type);

    if (payload_len == 0)
        return liquid_error(LIQUID_EICONFIG,"packet length must be greater than zero");

    // derived values
    unsigned int i;
    float        nstd  = powf(10.0f, noise_floor / 20.0f);           // noise std. dev.
    float        gamma = powf(10.0f, (SNRdB + noise_floor) / 20.0f); // channel gain

    // create dsssframegen object
    dsssframegenprops_s fgprops;
    fgprops.check   = check;
    fgprops.fec0    = fec0;
    fgprops.fec1    = fec1;
    dsssframegen fg = dsssframegen_create(&fgprops);

    // create dsssframesync object
    dsssframesync fs = dsssframesync_create(callback, NULL);
    if (debug_enabled) {
        // dsssframesync_debug_enable(fs);
    }

    // assemble the frame (NULL pointers for default values)
    dsssframegen_assemble(fg, NULL, NULL, payload_len);
    // dsssframegen_print(fg);

    // generate the frame in blocks
    unsigned int buf_len  = 256;
    float complex x[buf_len];
    float complex y[buf_len];

    int   frame_complete = 0;
    float phi            = 0.0f;
    while (!frame_complete) {
        frame_complete = dsssframegen_write_samples(fg, x, buf_len);

        // add noise and push through synchronizer
        for (i = 0; i < buf_len; i++) {
            // apply channel gain and carrier offset to input
            y[i] = gamma * x[i] * cexpf(_Complex_I * phi);
            phi += dphi;

            // add noise
            y[i] += nstd * (randnf() + _Complex_I * randnf()) * M_SQRT1_2;
        }

        // run through frame synchronizer
        dsssframesync_execute(fs, y, buf_len);
    }
    dsssframesync_print(fs);

    // export debugging file
    if (debug_enabled) {
        // dsssframesync_debug_print(fs, "dsssframesync_debug.m");
    }

    // dsssframesync_print(fs);
    // destroy allocated objects
    dsssframegen_destroy(fg);
    dsssframesync_destroy(fs);

    printf("done.\n");
    return 0;
}

static int callback(const unsigned char *  _header,
                    int                    _header_valid,
                    const unsigned char *  _payload,
                    unsigned int           _payload_len,
                    int                    _payload_valid,
                    framesyncstats_s       _stats,
                    void *                 _userdata)
{
    printf("******** callback invoked\n");

    // count bit errors (assuming all-zero message)
    unsigned int bit_errors = 0;
    unsigned int i;
    for (i = 0; i < _payload_len; i++)
        bit_errors += liquid_count_ones(_payload[i]);

    framesyncstats_print(&_stats);
    printf("    header crc          :   %s\n", _header_valid ? "pass" : "FAIL");
    printf("    payload length      :   %u\n", _payload_len);
    printf("    payload crc         :   %s\n", _payload_valid ? "pass" : "FAIL");
    printf("    payload bit errors  :   %u / %u\n", bit_errors, 8 * _payload_len);

    return 0;
}
