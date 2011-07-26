//
// fec_soft_example.c
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "liquid.h"

// print usage/help message
void usage()
{
    printf("fec_example [options]\n");
    printf("  u/h   : print usage\n");
    printf("  n     : input data size (number of uncoded bytes)\n");
    printf("  c     : coding scheme, (h74 default):\n");
    liquid_print_fec_schemes();
}


int main(int argc, char*argv[])
{
    // options
    unsigned int n = 4;                     // data length (bytes)
    unsigned int nmax = 2048;               // maximum data length
    fec_scheme fs = LIQUID_FEC_HAMMING74;   // error-correcting scheme

    int dopt;
    while((dopt = getopt(argc,argv,"uhn:c:")) != EOF){
        switch (dopt) {
        case 'h':
        case 'u': usage(); return 0;
        case 'n': n = atoi(optarg); break;
        case 'c':
            fs = liquid_getopt_str2fec(optarg);
            if (fs == LIQUID_FEC_UNKNOWN) {
                fprintf(stderr,"error: unknown/unsupported fec scheme \"%s\"\n\n",optarg);
                exit(1);
            }
            break;
        default:
            fprintf(stderr,"error: unknown option\n");
            exit(1);
        }
    }

    // ensure proper data length
    n = (n > nmax) ? nmax : n;

    // create arrays
    unsigned int n_enc = fec_get_enc_msg_length(fs,n);
    printf("dec msg len : %u\n", n);
    printf("enc msg len : %u\n", n_enc);
    unsigned char data[n];          // original data message
    unsigned char msg_enc[n_enc];   // encoded data message
    unsigned char msg_cor[8*n_enc]; // corrupted data message (soft bits)
    unsigned char msg_dec[n];       // decoded data message

    // create object
    fec q = fec_create(fs,NULL);
    fec_print(q);

    unsigned int i;

    // create message
    for (i=0; i<n; i++)
        data[i] = rand() & 0xff;

    // encode message
    fec_encode(q, n, data, msg_enc);

    // convert to soft bits and add 'noise'
    for (i=0; i<n_enc; i++) {
        msg_cor[8*i+0] = (msg_enc[i] & 0x80) ? 255 : 0;
        msg_cor[8*i+1] = (msg_enc[i] & 0x40) ? 255 : 0;
        msg_cor[8*i+2] = (msg_enc[i] & 0x20) ? 255 : 0;
        msg_cor[8*i+3] = (msg_enc[i] & 0x10) ? 255 : 0;
        msg_cor[8*i+4] = (msg_enc[i] & 0x08) ? 255 : 0;
        msg_cor[8*i+5] = (msg_enc[i] & 0x04) ? 255 : 0;
        msg_cor[8*i+6] = (msg_enc[i] & 0x02) ? 255 : 0;
        msg_cor[8*i+7] = (msg_enc[i] & 0x01) ? 255 : 0;
    }

    // flip first bit
    msg_cor[0] = 255 - msg_cor[0];

    // add noise
    for (i=0; i<8*n_enc; i++) {
        int soft_bit = 0.7*msg_cor[i] + (int)(100*randf());
        if (soft_bit > 255) soft_bit = 255;
        if (soft_bit <   0) soft_bit = 0;
        msg_cor[i] = soft_bit;
    }

    // decode message
    fec_decode_soft(q, n, msg_cor, msg_dec);

    printf("original message:  [%3u] ",n);
    for (i=0; i<n; i++)
        printf(" %.2X", (unsigned int) (data[i]));
    printf("\n");

    printf("encoded message:   [%3u] ",n_enc);
    for (i=0; i<n_enc; i++)
        printf(" %.2X", (unsigned int) (msg_enc[i]));
    printf("\n");

#if 0
    printf("corrupted message: [%3u]\n",n_enc);
    // print each soft bit value
    for (i=0; i<8*n_enc; i++) {
        if ( (i%8)==0 ) printf("%u:\n", i);

        div_t d = div(i,8);
        unsigned int bit_enc = (msg_enc[d.quot] >> (8-d.rem-1)) & 0x01;
        unsigned int bit_rec = (msg_cor[i] > 127) ? 1 : 0;
        printf("  %1u %3u (%1u) %c\n", bit_enc, msg_cor[i], bit_rec, bit_enc != bit_rec ? '*' : ' ');
    }
#else
    printf("corrupted message: [%3u] ",n_enc);
    // print compact bits
    for (i=0; i<n_enc; i++) {
        unsigned char byte = 0x00;

        byte |= (msg_cor[8*i+0] >> 0) & 0x80;
        byte |= (msg_cor[8*i+1] >> 1) & 0x40;
        byte |= (msg_cor[8*i+2] >> 2) & 0x20;
        byte |= (msg_cor[8*i+3] >> 3) & 0x10;
        byte |= (msg_cor[8*i+4] >> 4) & 0x08;
        byte |= (msg_cor[8*i+5] >> 5) & 0x04;
        byte |= (msg_cor[8*i+6] >> 6) & 0x02;
        byte |= (msg_cor[8*i+7] >> 7) & 0x01;

        printf("%c%.2X", byte==msg_enc[i] ? ' ' : '*', (unsigned int) (byte));
    }
#endif
    printf("\n");

    printf("decoded message:   [%3u] ",n);
    for (i=0; i<n; i++)
        printf("%c%.2X", msg_dec[i] == data[i] ? ' ' : '*', (unsigned int) (msg_dec[i]));
    printf("\n");
    printf("\n");

    // count bit errors
    unsigned int j, num_sym_errors=0, num_bit_errors=0;
    unsigned char e;
    for (i=0; i<n; i++) {
        num_sym_errors += (data[i] == msg_dec[i]) ? 0 : 1;

        e = data[i] ^ msg_dec[i];
        for (j=0; j<8; j++) {
            num_bit_errors += e & 0x01;
            e >>= 1;
        }
    }

    //printf("number of symbol errors detected: %d\n", num_errors_detected);
    printf("number of symbol errors received: %3u / %3u\n", num_sym_errors, n);
    printf("number of bit errors received:    %3u / %3u\n", num_bit_errors, n*8);

    // clean up objects
    fec_destroy(q);

    return 0;
}

