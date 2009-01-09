#ifndef __LIQUID_FEC_HAMMING74_AUTOTEST_H__
#define __LIQUID_FEC_HAMMING74_AUTOTEST_H__

#include "../../../autotest/autotest.h"
#include "../src/fec.h"

//
// AUTOTEST: Hamming (7,4) codec
//
void autotest_hamming74_codec()
{
    unsigned int n=4;
    unsigned char msg[] = {0x25, 0x62, 0x3F, 0x52};
    fec_scheme fs = FEC_HAMMING74;

    // create arrays
    unsigned int n_enc = fec_get_enc_msg_length(fs,n);
    unsigned char msg_dec[n];
    unsigned char msg_enc[n_enc];

    // create object
    fec q = fec_create(fs,NULL);
    fec_print(q);

    // encode message
    fec_encode(q, n, msg, msg_enc);
    
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

    // validate data are the same
    CONTEND_SAME_DATA(msg, msg_dec, n);

    // clean up objects
    fec_destroy(q);
}

#endif // __LIQUID_FEC_HAMMING74_AUTOTEST_H__

