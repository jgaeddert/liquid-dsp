// file: doc/listings/packetizer.example.c
#include <liquid/liquid.h>

int main() {
    // set up the options
    unsigned int n=16;                      // uncoded data length
    crc_scheme crc  = LIQUID_CRC_32;        // validity check
    fec_scheme fec0 = LIQUID_FEC_HAMMING74; // inner code
    fec_scheme fec1 = LIQUID_FEC_REP3;      // outer code

    // compute resulting packet length
    unsigned int k = packetizer_compute_enc_msg_len(n,crc,fec0,fec1);

    // set up the arrays
    unsigned char msg[n];       // original message
    unsigned char packet[k];    // encoded message
    unsigned char msg_dec[n];   // decoded message
    int crc_pass;               // decoder validity check

    // create the packetizer object
    packetizer p = packetizer_create(n,crc,fec0,fec1);

    // initialize msg here
    unsigned int i;
    for (i=0; i<n; i++) msg[i] = i & 0xff;

    // encode the packet
    packetizer_encode(p,msg,packet);

    // decode the packet, returning validity
    crc_pass = packetizer_decode(p,packet,msg_dec);

    // destroy the packetizer object
    packetizer_destroy(p);
}

