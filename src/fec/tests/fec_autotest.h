#ifndef __LIQUID_FEC_AUTOTEST_H__
#define __LIQUID_FEC_AUTOTEST_H__

#include "../../../autotest/autotest.h"
#include "../src/fec.h"

//
// AUTOTEST: Hamming (7,4) codec
//
void autotest_hamming74_codec()
{
    unsigned int n=4, num_errors_detected;
    unsigned char msg[] = {0xb5, 0x62, 0x3F, 0x52};

    unsigned char msg_enc[2*n];
    unsigned char msg_dec[n];

    // encode message
    fec_hamming74_encode(msg, n, msg_enc);
    
    // corrupt encoded message, flip one bit in
    // each encoded byte
    msg_enc[0] ^= 0x04;
    msg_enc[1] ^= 0x04;
    msg_enc[2] ^= 0x02;
    msg_enc[3] ^= 0x01;
    msg_enc[4] ^= 0x80;
    msg_enc[5] ^= 0x40;
    msg_enc[6] ^= 0x20;
    msg_enc[7] ^= 0x10;

    // decode message
    num_errors_detected =
        fec_hamming74_decode(msg_enc, n, msg_dec);

    // validate data are the same
    CONTEND_SAME_DATA(msg, msg_dec, n);
}

//
// AUTOTEST: repeat/3 codec
//
void autotest_rep3_codec()
{
    unsigned int n=4, num_errors_detected;
    unsigned char msg[] = {0xb5, 0x62, 0x3F, 0x52};

    unsigned char msg_enc[3*n];
    unsigned char msg_dec[n];

    // encode message
    fec_rep3_encode(msg, n, msg_enc);
    
    // corrupt encoded message, flip all bits in
    // first encoded block
    msg_enc[0] ^= 0xff;
    msg_enc[1] ^= 0xff;
    msg_enc[2] ^= 0xff;
    msg_enc[3] ^= 0xff;

    // decode message
    num_errors_detected =
        fec_rep3_decode(msg_enc, n, msg_dec);

    // validate data are the same
    CONTEND_SAME_DATA(msg, msg_dec, n);
}

#endif 

