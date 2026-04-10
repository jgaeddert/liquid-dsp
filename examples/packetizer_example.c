const char __docstr__[] =
"Demonstrates the functionality of the packetizer object.  Data are encoded"
" using two forward error-correction schemes (an inner and outer code) before"
" data errors are introduced.  The decoder then tries to recover the original"
" data message.";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

int main(int argc, char *argv[])
{
    // define variables and parse command-line options
    liquid_argparse_init(__docstr__);
    liquid_argparse_add(unsigned, msg_len_org, 8, 'n', "original message length [bytes]", NULL);
    liquid_argparse_add(char*,    crc, "crc32",   'v', "CRC scheme", liquid_argparse_crc);
    liquid_argparse_add(char*,    fs0, "g2412",   'c', "FEC scheme (inner)", liquid_argparse_fec);
    liquid_argparse_add(char*,    fs1,  "none",   'k', "FEC scheme (outer)", liquid_argparse_fec);
    liquid_argparse_parse(argc,argv);

    // create packetizer object
    crc_scheme check = (crc_scheme)liquid_getopt_str2crc(crc);
    fec_scheme fec0  = (fec_scheme)liquid_getopt_str2fec(fs0);
    fec_scheme fec1  = (fec_scheme)liquid_getopt_str2fec(fs1);
    unsigned int msg_len_enc = packetizer_compute_enc_msg_len(msg_len_org,check,fec0,fec1);
    packetizer p = packetizer_create(msg_len_org,check,fec0,fec1);
    packetizer_print(p);

    // initialize arrays
    LIQUID_VLA(unsigned char, msg_org, msg_len_org);   // original message
    LIQUID_VLA(unsigned char, msg_enc, msg_len_enc);   // encoded message
    LIQUID_VLA(unsigned char, msg_rec, msg_len_enc);   // received message
    LIQUID_VLA(unsigned char, msg_dec, msg_len_org);   // decoded message
    int crc_pass;

    // initialize original data message
    unsigned int i;
    for (i=0; i<msg_len_org; i++)
        msg_org[i] = rand() % 256;

    // encode packet
    packetizer_encode(p,msg_org,msg_enc);

    // add error(s)
    memmove(msg_rec, msg_enc, msg_len_enc*sizeof(unsigned char));
    msg_rec[0] ^= 0x01;

    // decode packet
    crc_pass =
    packetizer_decode(p,msg_rec,msg_dec);

    // clean up allocated objects
    packetizer_destroy(p);

    // print results
    printf("original message:  [%3u] ",msg_len_org);
    for (i=0; i<msg_len_org; i++)
        printf(" %.2X", (unsigned int) (msg_org[i]));
    printf("\n");

    printf("encoded message:   [%3u] ",msg_len_enc);
    for (i=0; i<msg_len_enc; i++)
        printf(" %.2X", (unsigned int) (msg_enc[i]));
    printf("\n");

    printf("received message:  [%3u] ",msg_len_enc);
    for (i=0; i<msg_len_enc; i++)
        printf("%c%.2X", msg_rec[i]==msg_enc[i] ? ' ' : '*', (unsigned int) (msg_rec[i]));
    printf("\n");

    printf("decoded message:   [%3u] ",msg_len_org);
    for (i=0; i<msg_len_org; i++)
        printf("%c%.2X", msg_dec[i] == msg_org[i] ? ' ' : '*', (unsigned int) (msg_dec[i]));
    printf("\n");
    printf("\n");

    // count bit errors
    unsigned int num_sym_errors=0;
    unsigned int num_bit_errors=0;
    for (i=0; i<msg_len_org; i++) {
        num_sym_errors += (msg_org[i] == msg_dec[i]) ? 0 : 1;

        num_bit_errors += count_bit_errors(msg_org[i], msg_dec[i]);
    }

    //printf("number of symbol errors detected: %d\n", num_errors_detected);
    printf("number of symbol errors received: %4u / %4u\n", num_sym_errors, msg_len_org);
    printf("number of bit errors received:    %4u / %4u\n", num_bit_errors, msg_len_org*8);
    printf("(crc %s)\n", crc_pass ? "passed" : "failed");
    return 0;
}

