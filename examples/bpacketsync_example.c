//
// bpacketsync_example.c
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "liquid.h"

int callback(unsigned char * _payload,
             int _payload_valid,
             unsigned int _payload_len,
             void * _userdata)
{
    printf("callback invoked, payload (%u bytes) : %s\n",
            _payload_len,
            _payload_valid ? "valid" : "INVALID!");
    return 0;
}

// print usage/help message
void usage()
{
    unsigned int i;
    printf("bpacketsync_example [options]\n");
    printf("  u/h   : print usage\n");
    printf("  n     : input data size (number of uncoded bytes): 8 default\n");
    printf("  v     : data integrity check: crc32 default\n");
    // print all available CRC schemes
    for (i=0; i<LIQUID_NUM_CRC_SCHEMES; i++)
        printf("          [%s] %s\n", crc_scheme_str[i][0], crc_scheme_str[i][1]);
    printf("  c     : coding scheme (inner): h74 default\n");
    printf("  k     : coding scheme (outer): none default\n");
    // print all available FEC schemes
    for (i=0; i<LIQUID_NUM_FEC_SCHEMES; i++)
        printf("          [%s] %s\n", fec_scheme_str[i][0], fec_scheme_str[i][1]);
}


int main(int argc, char*argv[]) {
    // options
    unsigned int n=8;                   // original data message length
    crc_scheme check = CRC_32;          // data integrity check
    fec_scheme fec0 = FEC_HAMMING74;    // inner code
    fec_scheme fec1 = FEC_NONE;         // outer code

    // read command-line options
    int dopt;
    while((dopt = getopt(argc,argv,"uhn:v:c:k:")) != EOF){
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
        case 'v':
            // data integrity check
            check = liquid_getopt_str2crc(optarg);
            if (check == CRC_UNKNOWN) {
                fprintf(stderr,"error: unknown/unsupported CRC scheme \"%s\"\n\n",optarg);
                exit(1);
            }
            break;
        case 'c':
            // inner FEC scheme
            fec0 = liquid_getopt_str2fec(optarg);
            if (fec0 == FEC_UNKNOWN) {
                fprintf(stderr,"error: unknown/unsupported inner FEC scheme \"%s\"\n\n",optarg);
                exit(1);
            }
            break;
        case 'k':
            // outer FEC scheme
            fec1 = liquid_getopt_str2fec(optarg);
            if (fec1 == FEC_UNKNOWN) {
                fprintf(stderr,"error: unknown/unsupported outer FEC scheme \"%s\"\n\n",optarg);
                exit(1);
            }
            break;
        default:
            fprintf(stderr,"error: unknown/invalid option\n");
            exit(1);
        }
    }

    // TODO: validate input

    // create packet generator
    bpacketgen pg = bpacketgen_create(0, n, check, fec0, fec1);

    // create packet synchronizer
    bpacketsync ps = bpacketsync_create(0, callback, NULL);

    unsigned int i;
    unsigned int k = bpacketgen_get_packet_len(pg);

    // initialize arrays
    unsigned char msg_org[n];   // original message
    unsigned char msg_enc[k];   // encoded message
    unsigned char msg_rec[k];   // recieved message
    unsigned char msg_dec[n];   // decoded message

    // initialize original data message
    for (i=0; i<n; i++)
        msg_org[i] = rand() % 256;

    // encode packet
    bpacketgen_encode(pg,msg_org,msg_enc);

    // add delay and error(s)
    memmove(msg_rec, msg_enc, k*sizeof(unsigned char));
    //msg_rec[0] ^= 0x01;

    // run packet synchronizer
    for (i=0; i<k; i++) {
        bpacketsync_execute(ps, msg_rec[i]);
    }

    // clean up allocated objects
    bpacketgen_destroy(pg);
    bpacketsync_destroy(ps);

    return 0;
}

