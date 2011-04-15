// file: doc/tutorials/fec_tutorial.c
#include <stdio.h>
#include <liquid/liquid.h>

int main() {
    // simulation parameters
    unsigned int n = 8;                     // original data length (bytes)
    fec_scheme fs = LIQUID_FEC_HAMMING74;   // error-correcting scheme

    // compute size of encoded message
    unsigned int k = fec_get_enc_msg_length(fs,n);

    // create arrays
    unsigned char msg_org[n];   // original data message
    unsigned char msg_enc[k];   // encoded/received data message
    unsigned char msg_dec[n];   // decoded data message

    // CREATE the fec object
    fec q = fec_create(fs,NULL);
    fec_print(q);

    unsigned int i;
    // generate message
    for (i=0; i<n; i++)
        msg_org[i] = i & 0xff;

    // encode message
    fec_encode(q, n, msg_org, msg_enc);

    // corrupt encoded message (flip bit)
    msg_enc[0] ^= 0x01;

    // decode message
    fec_decode(q, n, msg_enc, msg_dec);

    // DESTROY the fec object
    fec_destroy(q);

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

    printf("done.\n");
    return 0;
}
