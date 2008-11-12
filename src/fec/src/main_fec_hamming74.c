//
//
//

#include <stdio.h>

//#include "fec_internal.h"
#include "fec.h"

int main() {
    unsigned int n=4, num_errors_detected;
    unsigned char data[] = {0xb5, 0x62, 0x3F, 0x52};

    unsigned char msg_enc[2*n];
    unsigned char msg_dec[n];

    // encode message
    fec_hamming74_encode(data, n, msg_enc);
    
    unsigned int i;

    printf("original message:         \t");
    for (i=0; i<n; i++)
        printf("%.2x ", (unsigned int) (data[i]));
    printf("\n");

    printf("encoded message:          \t");
    for (i=0; i<2*n; i++)
        printf("%.2x ", (unsigned int) (msg_enc[i]));
    printf("\n");

    // corrupt encoded message
    msg_enc[0] ^= 0x04; // position 5
    msg_enc[1] ^= 0x04; //
    msg_enc[2] ^= 0x02; //
    msg_enc[3] ^= 0x01; //
    msg_enc[4] ^= 0x80; //
    msg_enc[5] ^= 0x40; //
    msg_enc[6] ^= 0x20; //
    msg_enc[7] ^= 0x10; //

    printf("corrupted message:        \t");
    for (i=0; i<2*n; i++)
        printf("%.2x ", (unsigned int) (msg_enc[i]));
    printf("\n");

    // decode message
    num_errors_detected =
        fec_hamming74_decode(msg_enc, n, msg_dec);

    printf("decoded message:          \t");
    for (i=0; i<n; i++)
        printf("%.2x ", (unsigned int) (msg_dec[i]));
    printf("\n");

    // count symbol errors
    unsigned int num_errors=0;
    for (i=0; i<n; i++)
        num_errors += (data[i]==msg_dec[i]) ? 0 : 1;

    printf("number of symbol errors detected: %d\n", num_errors_detected);
    printf("number of symbol errors received: %d\n", num_errors);

    return 0;
}
