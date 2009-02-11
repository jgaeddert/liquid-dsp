#ifndef __LIQUID_FEC_REP3_AUTOTEST_H__
#define __LIQUID_FEC_REP3_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

//
// AUTOTEST: repeat/3 codec
//
void autotest_rep3_codec()
{
    unsigned int n=4;
    unsigned char msg[] = {0x25, 0x62, 0x3F, 0x52};
    fec_scheme fs = FEC_REP3;

    // create arrays
    unsigned int n_enc = fec_get_enc_msg_length(fs,n);
    unsigned char msg_dec[n];
    unsigned char msg_enc[n_enc];

    // create object
    fec q = fec_create(fs,NULL);
    if (_autotest_verbose)
        fec_print(q);

    // encode message
    fec_encode(q, n, msg, msg_enc);
    
    // corrupt encoded message
    msg_enc[0] = ~msg_enc[0];
    msg_enc[1] = ~msg_enc[1];
    msg_enc[2] = ~msg_enc[2];
    msg_enc[3] = ~msg_enc[3];

    // decode message
    fec_decode(q, n, msg_enc, msg_dec);

    // validate data are the same
    CONTEND_SAME_DATA(msg, msg_dec, n);

    // clean up objects
    fec_destroy(q);
}

#endif // __LIQUID_FEC_REP3_AUTOTEST_H__

