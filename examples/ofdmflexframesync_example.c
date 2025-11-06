char __docstr__[] = "Example demonstrating the OFDM flexible frame synchronizer.";

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "liquid.h"
#include "liquid.argparse.h"

// callback function
int callback(const unsigned char *  _header,
             int                    _header_valid,
             const unsigned char *  _payload,
             unsigned int           _payload_len,
             int                    _payload_valid,
             framesyncstats_s       _stats,
             void *                 _userdata);

int main(int argc, char *argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(char*,    mod,      "qpsk", 'm', "FEC scheme", liquid_argparse_modem);
    liquid_argparse_add(char*,    crc,     "crc32", 'v', "CRC scheme", liquid_argparse_crc);
    liquid_argparse_add(char*,    fs0,      "none", 'c', "FEC scheme (inner)", liquid_argparse_fec);
    liquid_argparse_add(char*,    fs1,      "none", 'k', "FEC scheme (outer)", liquid_argparse_fec);
    liquid_argparse_add(unsigned, M,            64, 'M', "number of subcarriers", NULL);
    liquid_argparse_add(unsigned, cp_len,       16, 'C', "cyclic prefix length", NULL);
    liquid_argparse_add(unsigned, taper_len,     4, 'T', "taper length", NULL);
    liquid_argparse_add(unsigned, payload_len, 120, 'n', "data length (bytes)", NULL);
    liquid_argparse_add(float,    noise_floor, -60, '0', "noise floor [dB]", NULL);
    liquid_argparse_add(float,    SNRdB,        20, 's', "signal-to-noise ratio [dB]", NULL);
    liquid_argparse_add(float,    dphi,       0.02, 'f', "carrier frequency offset [radians/sample]", NULL);
    liquid_argparse_add(bool,     debug,     false, 'd', "enable debugging", NULL);
    liquid_argparse_parse(argc,argv);

    modulation_scheme ms    = liquid_getopt_str2mod(mod);
    crc_scheme        check = liquid_getopt_str2crc(crc);
    fec_scheme        fec0  = liquid_getopt_str2fec(fs0);
    fec_scheme        fec1  = liquid_getopt_str2fec(fs1);

    // derived values
    unsigned int  buf_len = 256;
    float complex buf[buf_len]; // time-domain buffer

    // allocate memory for header, payload
    unsigned char header[8];
    unsigned char payload[payload_len];

    // create frame generator
    ofdmflexframegenprops_s fgprops;
    ofdmflexframegenprops_init_default(&fgprops);
    fgprops.check           = check;
    fgprops.fec0            = fec0;
    fgprops.fec1            = fec1;
    fgprops.mod_scheme      = ms;
    ofdmflexframegen fg = ofdmflexframegen_create(M, cp_len, taper_len, NULL, &fgprops);

    // create frame synchronizer
    ofdmflexframesync fs = ofdmflexframesync_create(M, cp_len, taper_len, NULL, callback, (void*)payload);
    if (debug)
        ofdmflexframesync_debug_enable(fs);

    // initialize header/payload and assemble frame
    unsigned int i;
    for (i=0; i<8; i++)
        header[i] = i & 0xff;
    for (i=0; i<payload_len; i++)
        payload[i] = rand() & 0xff;
    ofdmflexframegen_assemble(fg, header, payload, payload_len);
    ofdmflexframegen_print(fg);
    ofdmflexframesync_print(fs);

    // create channel and add impairments
    channel_cccf channel = channel_cccf_create();
    channel_cccf_add_awgn(channel, noise_floor, SNRdB);
    channel_cccf_add_carrier_offset(channel, dphi, 0.0f);

    // generate frame, push through channel
    int last_symbol=0;
    while (!last_symbol) {
        // generate symbol
        last_symbol = ofdmflexframegen_write(fg, buf, buf_len);

        // apply channel to buffer (in place)
        channel_cccf_execute_block(channel, buf, buf_len, buf);

        // push samples through synchronizer
        ofdmflexframesync_execute(fs, buf, buf_len);
    }

    // export debugging file
    if (debug)
        ofdmflexframesync_debug_print(fs, "ofdmflexframesync_debug.m");

    ofdmflexframesync_print(fs);
    // destroy objects
    ofdmflexframegen_destroy(fg);
    ofdmflexframesync_destroy(fs);
    channel_cccf_destroy(channel);

    printf("done.\n");
    return 0;
}

// callback function
int callback(const unsigned char *  _header,
             int                    _header_valid,
             const unsigned char *  _payload,
             unsigned int           _payload_len,
             int                    _payload_valid,
             framesyncstats_s       _stats,
             void *                 _userdata)
{
    printf("**** callback invoked : rssi = %8.3f dB, evm = %8.3f dB, cfo = %8.5f\n", _stats.rssi, _stats.evm, _stats.cfo);

    unsigned int i;

    // print header data to standard output
    printf("  header rx  :");
    for (i=0; i<8; i++)
        printf(" %.2X", _header[i]);
    printf("\n");

    // print payload data to standard output
    printf("  payload rx :");
    for (i=0; i<_payload_len; i++) {
        printf(" %.2X", _payload[i]);
        if ( ((i+1)%26)==0 && i !=_payload_len-1 )
            printf("\n              ");
    }
    printf("\n");

    // count errors in received payload and print to standard output
    unsigned char * payload_tx = (unsigned char*) _userdata;
    unsigned int num_errors = count_bit_errors_array(_payload, payload_tx, _payload_len);
    printf("  bit errors : %u / %u\n", num_errors, 8*_payload_len);

    return 0;
}

