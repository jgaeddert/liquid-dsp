//
// Packetizer example
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>

#include "liquid.h"

// print usage/help message
void usage()
{
    printf("fec_example [options]\n");
    printf("  u/h   : print usage\n");
    printf("  n     : input data size (number of uncoded bytes): 16 default\n");
    printf("  c     : coding scheme (inner): h74 default\n");
    printf("  k     : coding scheme (outer): r3 default\n");
    // print all available FEC schemes
    unsigned int i;
    for (i=0; i<LIQUID_NUM_FEC_SCHEMES; i++)
        printf("          %s\n", fec_scheme_str[i]);
}


int main(int argc, char*argv[]) {
    // 
    unsigned int n=16;
    fec_scheme fec0 = FEC_HAMMING74;
    fec_scheme fec1 = FEC_REP3;

    int dopt;
    while((dopt = getopt(argc,argv,"uhn:c:k:")) != EOF){
        switch (dopt) {
        case 'h':
        case 'u': usage(); return 0;
        case 'n':
            n = atoi(optarg);
            if (n < 1) {
                printf("error: packet length must be positive\n");
                usage();
                exit(-1);
            }
            break;
        case 'c':
            // inner FEC scheme
            fec0 = liquid_getopt_str2fec(optarg);
            if (fec0 == FEC_UNKNOWN) {
                printf("error: unknown/unsupported modulation scheme \"%s\"\n\n",optarg);
                usage();
                exit(-1);
            }
            break;
        case 'k':
            // outer FEC scheme
            fec1 = liquid_getopt_str2fec(optarg);
            if (fec1 == FEC_UNKNOWN) {
                printf("error: unknown/unsupported modulation scheme \"%s\"\n\n",optarg);
                usage();
                exit(-1);
            }
            break;
        default:
            printf("error: unknown/invalid option\n");
            usage();
            exit(-1);
        }
    }

    unsigned int packet_len = packetizer_get_packet_length(n,fec0,fec1);
    packetizer p = packetizer_create(n,fec0,fec1);
    packetizer_print(p);

    unsigned char msg[n];
    unsigned char msg_dec[n];
    unsigned char packet[packet_len];
    bool crc_pass;

    unsigned int i;
    for (i=0; i<n; i++) {
        msg[i] = rand() % 256;
        msg_dec[i] = 0;
    }

    packetizer_encode(p,msg,packet);

    // add errors
    packet[0] ^= 0x03;

    crc_pass =
    packetizer_decode(p,packet,msg_dec);

    // print
    printf("msg (original) [%3u] : ", n);
    for (i=0; i<n; i++) printf("%.2X ", msg[i]);
    printf("\n");

    printf("msg (decoded)  [%3u] : ", n);
    for (i=0; i<n; i++) printf("%.2X ", msg_dec[i]);
    printf("\n");

    // count errors
    unsigned int num_sym_errors = 0;
    for (i=0; i<n; i++) {
        num_sym_errors += (msg[i] == msg_dec[i]) ? 0 : 1;
    }

    printf("decoded packet errors: %3u / %3u  ", num_sym_errors, n);
    if (crc_pass)
        printf("(crc passed)\n");
    else
        printf("(crc failed)\n");

    printf("done.\n");
    return 0;
}

