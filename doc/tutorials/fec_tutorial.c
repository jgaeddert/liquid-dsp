#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <liquid/liquid.h>

int main() {
    // options
    unsigned int msg_len_dec = 8;   // data length (bytes)
    fec_scheme fs = FEC_HAMMING74;  // error-correcting scheme

    // create arrays
    unsigned int msg_len_enc = fec_get_enc_msg_length(fs,msg_len_dec);
    printf("dec msg len : %u\n", msg_len_dec);
    printf("enc msg len : %u\n", msg_len_enc);
    unsigned char msg_org[msg_len_dec]; // original data message
    unsigned char msg_enc[msg_len_enc]; // encoded data message
    unsigned char msg_cor[msg_len_enc]; // corrupted data message
    unsigned char msg_dec[msg_len_dec]; // decoded data message

    // create object
    fec q = fec_create(fs,NULL);
    fec_print(q);

    unsigned int i;

    // create message
    for (i=0; i<msg_len_dec; i++)
        msg_org[i] = rand() & 0xff;

    // encode message
    fec_encode(q, msg_len_dec, msg_org, msg_enc);

    // corrupt encoded message
    memmove(msg_cor, msg_enc, msg_len_enc);
    msg_cor[0] ^= 0x04; // position 5
#if 0
    msg_cor[1] ^= 0x04; //
    msg_cor[2] ^= 0x02; //
    msg_cor[3] ^= 0x01; //
    msg_cor[4] ^= 0x80; //
    msg_cor[5] ^= 0x40; //
    msg_cor[6] ^= 0x20; //
    msg_cor[7] ^= 0x10; //
#endif

    // decode message
    fec_decode(q, msg_len_dec, msg_cor, msg_dec);

    printf("original message:  [%3u] ",msg_len_dec);
    for (i=0; i<msg_len_dec; i++)
        printf(" %.2X", (unsigned int) (msg_org[i]));
    printf("\n");

    printf("encoded message:   [%3u] ",msg_len_enc);
    for (i=0; i<msg_len_enc; i++)
        printf(" %.2X", (unsigned int) (msg_enc[i]));
    printf("\n");

    printf("corrupted message: [%3u] ",msg_len_enc);
    for (i=0; i<msg_len_enc; i++)
        printf("%c%.2X", msg_cor[i]==msg_enc[i] ? ' ' : '*', (unsigned int) (msg_cor[i]));
    printf("\n");

    printf("decoded message:   [%3u] ",msg_len_dec);
    for (i=0; i<msg_len_dec; i++)
        printf("%c%.2X", msg_dec[i] == msg_org[i] ? ' ' : '*', (unsigned int) (msg_dec[i]));
    printf("\n");
    printf("\n");

    // count bit errors
    unsigned int j, num_sym_errors=0, num_bit_errors=0;
    unsigned char e;
    for (i=0; i<msg_len_dec; i++) {
        num_sym_errors += (msg_org[i] == msg_dec[i]) ? 0 : 1;

        e = msg_org[i] ^ msg_dec[i];
        for (j=0; j<8; j++) {
            num_bit_errors += e & 0x01;
            e >>= 1;
        }
    }

    //printf("number of symbol errors detected: %d\n", num_errors_detected);
    printf("number of symbol errors received: %3u / %3u\n", num_sym_errors, msg_len_dec);
    printf("number of bit errors received:    %3u / %3u\n", num_bit_errors, msg_len_dec*8);

    // clean up objects
    fec_destroy(q);

    return 0;
}

