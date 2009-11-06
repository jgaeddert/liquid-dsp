//
// fec_example.c
//
// This example demonstrates the interface for forward
// error-correction (FEC) codes.  A buffer of data bytes
// is encoded and corrupted with several errors.  The
// decoder then attempts to recover the original data
// set.
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
    printf("  c     : coding scheme, [h74], r3, v27, v29, v39, v615,\n");
    printf("          v27p23, v27p34, v27p45, v27p56, v27p67, v27p78,\n");
    printf("          v29p23, v29p34, v29p45, v29p56, v29p67, v29p78,\n");
    printf("          none\n");
}


int main(int argc, char*argv[]) {
    unsigned int n = 4;             // data length (bytes)
    unsigned int nmax = 100;        // maximum data length
    fec_scheme fs = FEC_HAMMING74;  // error-correcting scheme

    int dopt;
    while((dopt = getopt(argc,argv,"uhc:n:")) != EOF){
        switch (dopt) {
        case 'h':
        case 'u': usage(); return 0;
        case 'n': n = atoi(optarg); break;
        case 'c':
            if (strcmp(optarg,"none")==0) {
                fs = FEC_NONE;

            // convolutional codes
            } else if (strcmp(optarg, "v27")==0) {
                fs = FEC_CONV_V27;
            } else if (strcmp(optarg, "v29")==0) {
                fs = FEC_CONV_V29;
            } else if (strcmp(optarg, "v39")==0) {
                fs = FEC_CONV_V39;
            } else if (strcmp(optarg, "v615")==0) {
                fs = FEC_CONV_V615;

            // punctured codes (K=7)
            } else if (strcmp(optarg, "v27p23")==0) {
                fs = FEC_CONV_V27P23;
            } else if (strcmp(optarg, "v27p34")==0) {
                fs = FEC_CONV_V27P34;
            } else if (strcmp(optarg, "v27p45")==0) {
                fs = FEC_CONV_V27P45;
            } else if (strcmp(optarg, "v27p56")==0) {
                fs = FEC_CONV_V27P56;
            } else if (strcmp(optarg, "v27p67")==0) {
                fs = FEC_CONV_V27P67;
            } else if (strcmp(optarg, "v27p78")==0) {
                fs = FEC_CONV_V27P78;

            // punctured codes (K=9)
            } else if (strcmp(optarg, "v29p23")==0) {
                fs = FEC_CONV_V29P23;
            } else if (strcmp(optarg, "v29p34")==0) {
                fs = FEC_CONV_V29P34;
            } else if (strcmp(optarg, "v29p45")==0) {
                fs = FEC_CONV_V29P45;
            } else if (strcmp(optarg, "v29p56")==0) {
                fs = FEC_CONV_V29P56;
            } else if (strcmp(optarg, "v29p67")==0) {
                fs = FEC_CONV_V29P67;
            } else if (strcmp(optarg, "v29p78")==0) {
                fs = FEC_CONV_V29P78;

            // repeat code (3)
            } else if (strcmp(optarg, "r3")==0) {
                fs = FEC_REP3;

            // Hamming (7,4) block code
            } else if (strcmp(optarg, "h74")==0) {
                fs = FEC_HAMMING74;
            } else {
                printf("error: unknown modulation scheme \"%s\"\n\n",optarg);
                usage();
                exit(-1);
            }
            break;
        default:
            printf("error: unknown option\n");
            exit(-1);
        }
    }

    // ensure proper data length
    n = (n > nmax) ? nmax : n;

    // create arrays
    unsigned int n_enc = fec_get_enc_msg_length(fs,n);
    unsigned char data[n];          // original data message
    unsigned char msg_enc[n_enc];   // encoded data message
    unsigned char msg_cor[n_enc];   // corrupted data message
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

    // corrupt encoded message
    memmove(msg_cor, msg_enc, n_enc);
    msg_cor[0] ^= 0x04; // position 5
    /*
    msg_cor[1] ^= 0x04; //
    msg_cor[2] ^= 0x02; //
    msg_cor[3] ^= 0x01; //
    msg_cor[4] ^= 0x80; //
    msg_cor[5] ^= 0x40; //
    msg_cor[6] ^= 0x20; //
    msg_cor[7] ^= 0x10; //
    */

    // decode message
    fec_decode(q, n, msg_cor, msg_dec);

    printf("original message:  [%3u] ",n);
    for (i=0; i<n; i++)
        printf(" %.2X", (unsigned int) (data[i]));
    printf("\n");

    printf("encoded message:   [%3u] ",n_enc);
    for (i=0; i<n_enc; i++)
        printf(" %.2X", (unsigned int) (msg_enc[i]));
    printf("\n");

    printf("corrupted message: [%3u] ",n_enc);
    for (i=0; i<n_enc; i++)
        printf("%c%.2X", msg_cor[i]==msg_enc[i] ? ' ' : '*', (unsigned int) (msg_cor[i]));
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

