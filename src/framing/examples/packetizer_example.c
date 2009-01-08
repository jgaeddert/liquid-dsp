//
// Packetizer example
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../src/framing.h"

#include "../../fec/src/fec.h"

int main() {
    // 
    unsigned int n=16;
    fec_scheme fec0 = FEC_HAMMING74;
    fec_scheme fec1 = FEC_REP3;

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

