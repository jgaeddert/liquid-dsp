//
//
//

#include <stdio.h>

#include "liquid.h"

int main() {
    unsigned int n=4;
    unsigned char data[] = {0x25, 0x62, 0x3F, 0x52};
    fec_scheme fs = FEC_HAMMING74;

    // create arrays
    unsigned int n_enc = fec_get_enc_msg_length(fs,n);
    unsigned char msg_dec[n];
    unsigned char msg_enc[n_enc];

    // create object
    fec q = fec_create(fs,NULL);
    fec_print(q);

    // encode message
    fec_encode(q, n, data, msg_enc);
    
    // corrupt encoded message
    msg_enc[0] ^= 0x04; // position 5
    msg_enc[1] ^= 0x04; //
    msg_enc[2] ^= 0x02; //
    msg_enc[3] ^= 0x01; //
    msg_enc[4] ^= 0x80; //
    msg_enc[5] ^= 0x40; //
    msg_enc[6] ^= 0x20; //
    msg_enc[7] ^= 0x10; //

    // decode message
    fec_decode(q, n, msg_enc, msg_dec);

    unsigned int i;

    printf("original message:           ");
    for (i=0; i<n; i++)
        printf("%.2X ", (unsigned int) (data[i]));
    printf("\n");

    printf("encoded/corrupted message:  ");
    for (i=0; i<n_enc; i++)
        printf("%.2X ", (unsigned int) (msg_enc[i]));
    printf("\n");

    printf("decoded message:            ");
    for (i=0; i<n; i++)
        printf("%.2X ", (unsigned int) (msg_dec[i]));
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
    printf("number of symbol errors received: %u\n", num_sym_errors);
    printf("number of bit errors received:    %u\n", num_bit_errors);

    // clean up objects
    fec_destroy(q);

    return 0;
}
