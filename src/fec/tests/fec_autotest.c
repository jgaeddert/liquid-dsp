/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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

#include <stdlib.h>

#include "autotest/autotest.h"
#include "liquid.internal.h"

// Helper function to keep code base small
void fec_test_codec(fec_scheme _fs, unsigned int _n, void * _opts)
{
#if !LIBFEC_ENABLED
    if ( _fs == LIQUID_FEC_CONV_V27    ||
         _fs == LIQUID_FEC_CONV_V29    ||
         _fs == LIQUID_FEC_CONV_V39    ||
         _fs == LIQUID_FEC_CONV_V615   ||
         _fs == LIQUID_FEC_CONV_V27P23 ||
         _fs == LIQUID_FEC_CONV_V27P34 ||
         _fs == LIQUID_FEC_CONV_V27P45 ||
         _fs == LIQUID_FEC_CONV_V27P56 ||
         _fs == LIQUID_FEC_CONV_V27P67 ||
         _fs == LIQUID_FEC_CONV_V27P78 ||
         _fs == LIQUID_FEC_CONV_V29P23 ||
         _fs == LIQUID_FEC_CONV_V29P34 ||
         _fs == LIQUID_FEC_CONV_V29P45 ||
         _fs == LIQUID_FEC_CONV_V29P56 ||
         _fs == LIQUID_FEC_CONV_V29P67 ||
         _fs == LIQUID_FEC_CONV_V29P78 ||
         _fs == LIQUID_FEC_RS_M8)
    {
        AUTOTEST_WARN("convolutional, Reed-Solomon codes unavailable (install libfec)\n");
        return;
    }
#endif

    // generate fec object
    fec q = fec_create(_fs,_opts);

    // create arrays
    unsigned int n_enc = fec_get_enc_msg_length(_fs,_n);
    unsigned char msg[_n];          // original message
    unsigned char msg_enc[n_enc];   // encoded message
    unsigned char msg_dec[_n];      // decoded message

    // initialze message
    unsigned int i;
    for (i=0; i<_n; i++) {
        msg[i] = rand() & 0xff;
        msg_dec[i] = 0;
    }

    // encode message
    fec_encode(q,_n,msg,msg_enc);

    // channel: add single error
    msg_enc[0] ^= 0x01;

    // decode message
    fec_decode(q,_n,msg_enc,msg_dec);

    // validate output
    CONTEND_SAME_DATA(msg,msg_dec,_n);

    // clean up objects
    fec_destroy(q);
}

// 
// AUTOTESTS: basic encode/decode functionality
//

// repeat codes
void autotest_fec_r3()      { fec_test_codec(LIQUID_FEC_REP3,          64, NULL); }
void autotest_fec_r5()      { fec_test_codec(LIQUID_FEC_REP5,          64, NULL); }

// Hamming block codes
void autotest_fec_h74()     { fec_test_codec(LIQUID_FEC_HAMMING74,     64, NULL); }
void autotest_fec_h84()     { fec_test_codec(LIQUID_FEC_HAMMING84,     64, NULL); }
void autotest_fec_h128()    { fec_test_codec(LIQUID_FEC_HAMMING128,    64, NULL); }

// Golay block codes
void autotest_fec_g2412()   { fec_test_codec(LIQUID_FEC_GOLAY2412,     64, NULL); }

// SEC-DED block codecs
void autotest_fec_secded2216() { fec_test_codec(LIQUID_FEC_SECDED2216, 64, NULL); }
void autotest_fec_secded3932() { fec_test_codec(LIQUID_FEC_SECDED3932, 64, NULL); }
void autotest_fec_secded7264() { fec_test_codec(LIQUID_FEC_SECDED7264, 64, NULL); }

// convolutional codes
void autotest_fec_v27()     { fec_test_codec(LIQUID_FEC_CONV_V27,      64, NULL); }
void autotest_fec_v29()     { fec_test_codec(LIQUID_FEC_CONV_V29,      64, NULL); }
void autotest_fec_v39()     { fec_test_codec(LIQUID_FEC_CONV_V39,      64, NULL); }
void autotest_fec_v615()    { fec_test_codec(LIQUID_FEC_CONV_V615,     64, NULL); }

// convolutional codes (punctured)
void autotest_fec_v27p23()  { fec_test_codec(LIQUID_FEC_CONV_V27P23,   64, NULL); }
void autotest_fec_v27p34()  { fec_test_codec(LIQUID_FEC_CONV_V27P34,   64, NULL); }
void autotest_fec_v27p45()  { fec_test_codec(LIQUID_FEC_CONV_V27P45,   64, NULL); }
void autotest_fec_v27p56()  { fec_test_codec(LIQUID_FEC_CONV_V27P56,   64, NULL); }
void autotest_fec_v27p67()  { fec_test_codec(LIQUID_FEC_CONV_V27P67,   64, NULL); }
void autotest_fec_v27p78()  { fec_test_codec(LIQUID_FEC_CONV_V27P78,   64, NULL); }

void autotest_fec_v29p23()  { fec_test_codec(LIQUID_FEC_CONV_V29P23,   64, NULL); }
void autotest_fec_v29p34()  { fec_test_codec(LIQUID_FEC_CONV_V29P34,   64, NULL); }
void autotest_fec_v29p45()  { fec_test_codec(LIQUID_FEC_CONV_V29P45,   64, NULL); }
void autotest_fec_v29p56()  { fec_test_codec(LIQUID_FEC_CONV_V29P56,   64, NULL); }
void autotest_fec_v29p67()  { fec_test_codec(LIQUID_FEC_CONV_V29P67,   64, NULL); }
void autotest_fec_v29p78()  { fec_test_codec(LIQUID_FEC_CONV_V29P78,   64, NULL); }

// Reed-Solomon block codes
void autotest_fec_rs8()     { fec_test_codec(LIQUID_FEC_RS_M8,         64, NULL); }


