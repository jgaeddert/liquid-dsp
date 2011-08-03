// file: doc/listings/interleaver.example.c
#include <liquid/liquid.h>

int main() {
    // options
    unsigned int n=9; // message length (bytes)

    // create the interleaver
    interleaver q = interleaver_create(n);
    interleaver_set_depth(q,3);

    // create arrays
    unsigned char msg_org[n];   // original message data
    unsigned char msg_int[n];   // interleaved data
    unsigned char msg_rec[n];   // de-interleaved, recovered data

    // ...initialize msg_org...

    // interleave/de-interleave the data
    interleaver_encode(q, msg_org, msg_int);
    interleaver_decode(q, msg_int, msg_rec);

    // destroy the interleaver object
    interleaver_destroy(q);
}
