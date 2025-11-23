char __docstr__[] = "Demonstrate bpacketsync interface.";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "liquid.h"
#include "liquid.argparse.h"

int callback(unsigned char *  _payload,
             int              _payload_valid,
             unsigned int     _payload_len,
             framesyncstats_s _stats,
             void *           _userdata)
{
    printf("callback invoked, payload (%u bytes) : %s\n",
            _payload_len,
            _payload_valid ? "valid" : "INVALID!");

    // copy data if valid
    if (_payload_valid) {
        unsigned char * msg_dec = (unsigned char*) _userdata;

        memmove(msg_dec, _payload, _payload_len*sizeof(unsigned char));
    }

    return 0;
}

int main(int argc, char* argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(unsigned, msg_len_org,    8,       'n', "original data message length", NULL);
    liquid_argparse_add(char*,    crc_type,       "crc32", 'v', "data integrity check", liquid_argparse_crc);
    liquid_argparse_add(char*,    fec0_type,      "h74",   'c', "inner code", liquid_argparse_fec);
    liquid_argparse_add(char*,    fec1_type,      "none",  'k', "outer code", liquid_argparse_fec);
    liquid_argparse_add(float,    bit_error_rate, 0.0f,    'e', "bit error rate", NULL);
    liquid_argparse_parse(argc,argv);

    // validate options
    crc_scheme crc  = liquid_getopt_str2crc(crc_type);
    fec_scheme fec0 = liquid_getopt_str2fec(fec0_type);
    fec_scheme fec1 = liquid_getopt_str2fec(fec1_type);

    if (msg_len_org == 0)
        return liquid_error(LIQUID_EICONFIG,"packet length must be greater than zero");
    if (bit_error_rate < 0.0f || bit_error_rate > 1.0f)
        return liquid_error(LIQUID_EICONFIG,"channel bit error rate must be in [0,1]");

    // create packet generator
    bpacketgen pg = bpacketgen_create(0, msg_len_org, crc, fec0, fec1);
    bpacketgen_print(pg);

    unsigned int i;

    // compute packet length
    unsigned int msg_len_enc = bpacketgen_get_packet_len(pg);

    // initialize arrays
    unsigned char msg_org[msg_len_org  ];   // original message
    unsigned char msg_enc[msg_len_enc  ];   // encoded message
    unsigned char msg_rec[msg_len_enc+1];   // received message
    unsigned char msg_dec[msg_len_org  ];   // decoded message

    // create packet synchronizer
    bpacketsync ps = bpacketsync_create(0, callback, (void*)msg_dec);
    bpacketsync_print(ps);

    // initialize original data message
    for (i=0; i<msg_len_org; i++)
        msg_org[i] = rand() % 256;

    // encode packet
    bpacketgen_encode(pg,msg_org,msg_enc);

    // channel: add delay
    msg_rec[0] = rand() & 0xff; // initialize first byte as random
    memmove(&msg_rec[1], msg_enc, msg_len_enc*sizeof(unsigned char));
    liquid_lbshift(msg_rec, (msg_len_enc+1)*sizeof(unsigned char), rand()%8); // random shift
    // add random errors
    for (i=0; i<msg_len_enc+1; i++) {
        unsigned int j;
        for (j=0; j<8; j++) {
            if (randf() < bit_error_rate)
                msg_rec[i] ^= 1 << (8-j-1);
        }
    }

    // run packet synchronizer

    // push random bits through synchronizer
    for (i=0; i<100; i++)
        bpacketsync_execute_byte(ps, rand() & 0xff);

    // push packet through synchronizer
    for (i=0; i<msg_len_enc+1; i++)
        bpacketsync_execute_byte(ps, msg_rec[i]);

    // count errors
    unsigned int num_bit_errors = 0;
    for (i=0; i<msg_len_org; i++)
        num_bit_errors += count_bit_errors(msg_org[i], msg_dec[i]);
    printf("number of bit errors received:    %4u / %4u\n", num_bit_errors, msg_len_org*8);

    // clean up allocated objects
    bpacketgen_destroy(pg);
    bpacketsync_destroy(ps);

    return 0;
}

