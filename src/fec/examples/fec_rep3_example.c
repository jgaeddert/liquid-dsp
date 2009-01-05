//
//
//

#include <stdio.h>

#include "../src/fec.h"

int main() {
    unsigned int n=4, num_errors_detected;
    unsigned char data[] = {0x25, 0x62, 0x3F, 0x52};

    unsigned char msg_enc[3*n];
    unsigned char msg_dec[n];

    // encode message
    fec_rep3_encode(data, n, msg_enc);
    
    // corrupt encoded message
    msg_enc[0] = ~msg_enc[0];
    msg_enc[1] = ~msg_enc[1];
    msg_enc[2] = ~msg_enc[2];
    msg_enc[3] = ~msg_enc[3];

    // decode message
    num_errors_detected =
        fec_rep3_decode(msg_enc, n, msg_dec);

    unsigned int i;

    printf("original message:         \t");
    for (i=0; i<n; i++)
        printf("%X ", (unsigned int) (data[i]));
    printf("\n");

    printf("encoded/corrupted message:\t");
    for (i=0; i<3*n; i++)
        printf("%X ", (unsigned int) (msg_enc[i]));
    printf("\n");

    printf("decoded message:          \t");
    for (i=0; i<n; i++)
        printf("%X ", (unsigned int) (msg_dec[i]));
    printf("\n");

    // count symbol errors
    unsigned int num_errors=0;
    for (i=0; i<n; i++)
        num_errors += (data[i]==msg_dec[i]) ? 0 : 1;

    printf("number of symbol errors detected: %d\n", num_errors_detected);
    printf("number of symbol errors received: %d\n", num_errors);

    return 0;
}
