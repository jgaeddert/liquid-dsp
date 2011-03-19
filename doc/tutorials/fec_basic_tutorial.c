// file: doc/tutorials/fec_basic_tutorial.c
#include <stdio.h>
#include <liquid/liquid.h>

int main() {
    // simulation parameters
    unsigned int n = 8;             // original data length (bytes)

    // compute size of encoded message
    unsigned int k = n;             // (no encoding yet)

    // create arrays
    unsigned char msg_org[n];       // original data message
    unsigned char msg_enc[k];       // encoded/received data message
    unsigned char msg_dec[n];       // decoded data message

    unsigned int i;
    // create message
    for (i=0; i<n; i++) msg_org[i] = i & 0xff;

    // "encode" message (copy to msg_enc)
    for (i=0; i<n; i++) msg_enc[i] = msg_org[i];

    // corrupt encoded message (flip bit)
    msg_enc[0] ^= 0x01;

    // "decode" message (copy to msg_dec)
    for (i=0; i<n; i++) msg_dec[i] = msg_enc[i];

    printf("original message:  [%3u] ",n);
    for (i=0; i<n; i++)
        printf(" %.2X", msg_org[i]);
    printf("\n");

    printf("decoded message:   [%3u] ",n);
    for (i=0; i<n; i++)
        printf(" %.2X", msg_dec[i]);
    printf("\n");

    // count bit errors
    unsigned int num_bit_errors = count_bit_errors_array(msg_org, msg_dec, n);
    printf("number of bit errors received:    %3u / %3u\n", num_bit_errors, n*8);

    return 0;
}
