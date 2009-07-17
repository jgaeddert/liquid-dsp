/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __LIQUID_FEC_CONV_AUTOTEST_H__
#define __LIQUID_FEC_CONV_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.internal.h"

//
// AUTOTEST: convolutional codecs
//
void conv_test(fec_scheme _fs,
               unsigned int _dec_msg_len)
{
#if HAVE_FEC_H==0
    printf("warning: convolutional codes unavailable\n");
    return;
#endif

    unsigned char msg[_dec_msg_len];
    unsigned int i;
    for (i=0; i<_dec_msg_len; i++)
        msg[i] = i & 0xff;

    // create arrays
    unsigned int _enc_msg_len = fec_get_enc_msg_length(_fs,_dec_msg_len);
    unsigned char msg_dec[_dec_msg_len];
    unsigned char msg_enc[_enc_msg_len];

    // create object
    fec q = fec_create(_fs,NULL);
    if (_autotest_verbose)
        fec_print(q);

    // encode message
    fec_encode(q, _dec_msg_len, msg, msg_enc);
    
    // corrupt encoded message
    //for (i=0; i<_enc_msg_len; i++)
    //    msg_enc[i] ^= 0x01;
    msg_enc[0] ^= 0x01;

    // decode message
    fec_decode(q, _dec_msg_len, msg_enc, msg_dec);

    // validate data are the same
    CONTEND_SAME_DATA(msg, msg_dec, _dec_msg_len);

    if (_autotest_verbose) {
        printf("enc   dec\n");
        printf("---   ---\n");
        for (i=0; i<_dec_msg_len; i++)
            printf("%3u   %3u\n", msg[i], msg_dec[i]);
    }

    // clean up objects
    fec_destroy(q);
}

void autotest_conv27_codec()    { conv_test(FEC_CONV_V27,   8); }
void autotest_conv29_codec()    { conv_test(FEC_CONV_V29,   8); }
void autotest_conv39_codec()    { conv_test(FEC_CONV_V39,   8); }
void autotest_conv615_codec()   { conv_test(FEC_CONV_V615,  8); }

// punctured codes
void autotest_conv27p23_codec() { conv_test(FEC_CONV_V27P23,8); }
void autotest_conv27p34_codec() { conv_test(FEC_CONV_V27P34,8); }
void autotest_conv27p45_codec() { conv_test(FEC_CONV_V27P45,8); }
void autotest_conv27p56_codec() { conv_test(FEC_CONV_V27P56,8); }
void autotest_conv27p67_codec() { conv_test(FEC_CONV_V27P67,8); }
void autotest_conv27p78_codec() { conv_test(FEC_CONV_V27P78,8); }

void autotest_conv29p23_codec() { conv_test(FEC_CONV_V29P23,8); }
void autotest_conv29p34_codec() { conv_test(FEC_CONV_V29P34,8); }
void autotest_conv29p45_codec() { conv_test(FEC_CONV_V29P45,8); }
void autotest_conv29p56_codec() { conv_test(FEC_CONV_V29P56,8); }
void autotest_conv29p67_codec() { conv_test(FEC_CONV_V29P67,8); }
void autotest_conv29p78_codec() { conv_test(FEC_CONV_V29P78,8); }

#endif // __LIQUID_FEC_CONV_AUTOTEST_H__

