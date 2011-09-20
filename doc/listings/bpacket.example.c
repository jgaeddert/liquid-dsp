// file: doc/listings/bpacket.example.c
#include <liquid/liquid.h>

int callback(unsigned char *  _payload,
             int              _payload_valid,
             unsigned int     _payload_len,
             framesyncstats_s _stats,
             void *           _userdata)
{
    printf("callback invoked\n");
    return 0;
}

int main() {
    // options
    unsigned int n=64;                       // original data message length
    crc_scheme check = LIQUID_CRC_32;        // data integrity check
    fec_scheme fec0 = LIQUID_FEC_HAMMING128; // inner code
    fec_scheme fec1 = LIQUID_FEC_NONE;       // outer code

    // create packet generator and compute packet length
    bpacketgen pg = bpacketgen_create(0, n, check, fec0, fec1);
    unsigned int k = bpacketgen_get_packet_len(pg);

    // initialize arrays
    unsigned char msg_org[n];   // original message
    unsigned char msg_enc[k];   // encoded message
    unsigned char msg_dec[n];   // decoded message

    // create packet synchronizer
    bpacketsync ps = bpacketsync_create(0, callback, NULL);

    // initialize original data message
    unsigned int i;
    for (i=0; i<n; i++) msg_org[i] = rand() % 256;

    // encode packet
    bpacketgen_encode(pg, msg_org, msg_enc);

    // ... channel ...

    // push packet through synchronizer
    bpacketsync_execute(ps, msg_enc, k);

    // clean up allocated objects
    bpacketgen_destroy(pg);
    bpacketsync_destroy(ps);
}

