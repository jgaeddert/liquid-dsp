//
// Packetizer example
//

#include <stdio.h>
#include <stdlib.h>

#include "../src/framing.h"

#include "../../fec/src/fec.h"

int main() {
    // 
    unsigned int n=16;

    packetizer p = packetizer_create(n,FEC_HAMMING74,FEC_HAMMING74);
    unsigned int packet_len = packetizer_get_packet_length(p);

    unsigned char msg[n];
    unsigned char msg_dec[n];
    unsigned char packet[packet_len];

    unsigned int i;
    for (i=0; i<n; i++) {
        msg[i] = rand() % 256;
    }

    packetizer_encode(p,msg,packet);

    packetizer_print(p);

    // add errors
    packet[0] ^= 0x03;

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

    printf("decoded packet errors: %3u / %3u\n", num_sym_errors, n);

    printf("done.\n");
    return 0;
}

