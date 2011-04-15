#include <stdio.h>
#include <liquid/liquid.h>

int main() {
    // simulation parameters
    unsigned int n = 8;                         // original data length (bytes)
    crc_scheme check = LIQUID_CRC_16;           // data validity check
    fec_scheme fec0 = LIQUID_FEC_HAMMING74;     // error-correcting scheme (inner)
    fec_scheme fec1 = LIQUID_FEC_HAMMING128;    // error-correcting scheme (outer)

    // compute size of encoded message
    unsigned int k = packetizer_compute_enc_msg_len(n,check,fec0,fec1);

    // create arrays
    unsigned char msg_org[n];       // original data message
    unsigned char msg_enc[k];       // encoded/received data message
    unsigned char msg_dec[n];       // decoded data message

    // CREATE the fec object
    packetizer q = packetizer_create(n,check,fec0,fec1);
    packetizer_print(q);

    unsigned int i;
    // generate message
    for (i=0; i<n; i++)
        msg_org[i] = i & 0xff;

    // encode message
    packetizer_encode(q, msg_org, msg_enc);

    // corrupt encoded message (flip several bits)
    msg_enc[0] ^= 0x03;

    // decode message
    packetizer_decode(q, msg_enc, msg_dec);

    // DESTROY the fec object
    packetizer_destroy(q);

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
