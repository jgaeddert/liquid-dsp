const char __docstr__[] =
"This example demonstrates the functionality of the packetizer object"
" for soft-decision decoding."
" Data are encoded using two forward error-correction schemes"
" (an inner and outer code) before noise and data"
" errors are added. The decoder then tries to recover the original data"
" message. Only the outer code uses soft-decision decoding.";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "liquid.h"
#include "liquid_vla.h"
#include "liquid.argparse.h"

// print usage/help message
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
    LIQUID_VLA(unsigned char, msg_rec, msg_len_enc*8); // received message (soft bits)
    LIQUID_VLA(unsigned char, msg_dec, msg_len_org);   // decoded message
    int crc_pass;

    // initialize original data message
    unsigned int i;
    for (i=0; i<msg_len_org; i++)
        msg_org[i] = rand() % 256;

    // encode packet
    packetizer_encode(p,msg_org,msg_enc);

    // convert to soft bits and add 'noise'
    for (i=0; i<msg_len_enc; i++) {
        msg_rec[8*i+0] = (msg_enc[i] & 0x80) ? 255 : 0;
        msg_rec[8*i+1] = (msg_enc[i] & 0x40) ? 255 : 0;
        msg_rec[8*i+2] = (msg_enc[i] & 0x20) ? 255 : 0;
        msg_rec[8*i+3] = (msg_enc[i] & 0x10) ? 255 : 0;
        msg_rec[8*i+4] = (msg_enc[i] & 0x08) ? 255 : 0;
        msg_rec[8*i+5] = (msg_enc[i] & 0x04) ? 255 : 0;
        msg_rec[8*i+6] = (msg_enc[i] & 0x02) ? 255 : 0;
        msg_rec[8*i+7] = (msg_enc[i] & 0x01) ? 255 : 0;
    }

    // flip first bit (ensure error)
    msg_rec[0] = 255 - msg_rec[0];

    // add noise (but not so much that it would cause a bit error)
    for (i=0; i<8*msg_len_enc; i++) {
        int soft_bit = msg_rec[i] + (int)(20*randnf());
        if (soft_bit > 255) soft_bit = 255;
        if (soft_bit <   0) soft_bit = 0;
        msg_rec[i] = soft_bit;
    }

    // decode packet
    crc_pass =
    packetizer_decode_soft(p,msg_rec,msg_dec);

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

    // print expanded result (print each soft bit value)
    for (i=0; i<msg_len_enc; i++)
    {
        unsigned char msg_cor_hard = 0x00;
        printf("%5u: ", i);
        unsigned int j;
        for (j=0; j<8; j++) {
            msg_cor_hard |= (msg_rec[8*i+j] > 127) ? 1<<(8-j-1) : 0;
            unsigned int bit_enc = (msg_enc[i] >> (8-j-1)) & 0x01;
            unsigned int bit_rec = (msg_rec[8*i+j] > 127) ? 1 : 0;
            //printf("%1u %3u (%1u) %c", bit_enc, msg_rec[i], bit_rec, bit_enc != bit_rec ? '*' : ' ');
            printf("%4u%c", msg_rec[8*i+j], bit_enc != bit_rec ? '*' : ' ');
        }
        printf("  :  %c%.2X  %.2X\n", msg_cor_hard==msg_enc[i] ? ' ' : '*', (unsigned int) (msg_cor_hard), msg_enc[i]);
    }

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

