/*
 * Copyright (c) 2007 - 2026 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdlib.h>

#include "liquid.autotest.h"
#include "liquid.internal.h"

// Helper function to keep code base small
void testbench_fec_copy(liquid_autotest __q__, fec_scheme _fs)
{
#if !LIBFEC_ENABLED
    switch (_fs) {
    case LIQUID_FEC_CONV_V27:
    case LIQUID_FEC_CONV_V29:
    case LIQUID_FEC_CONV_V39:
    case LIQUID_FEC_CONV_V615:
    case LIQUID_FEC_CONV_V27P23:
    case LIQUID_FEC_CONV_V27P34:
    case LIQUID_FEC_CONV_V27P45:
    case LIQUID_FEC_CONV_V27P56:
    case LIQUID_FEC_CONV_V27P67:
    case LIQUID_FEC_CONV_V27P78:
    case LIQUID_FEC_CONV_V29P23:
    case LIQUID_FEC_CONV_V29P34:
    case LIQUID_FEC_CONV_V29P45:
    case LIQUID_FEC_CONV_V29P56:
    case LIQUID_FEC_CONV_V29P67:
    case LIQUID_FEC_CONV_V29P78:
    case LIQUID_FEC_RS_M8:
        LIQUID_WARN("convolutional, Reed-Solomon codes unavailable (install libfec)");
        return;
    default:;
    }
#endif
    unsigned int n_dec = 64;
    void *       opts  = NULL;

    // generate fec object
    fec q0 = fec_create(_fs,opts);

    // create arrays
    unsigned int n_enc = fec_get_enc_msg_length(_fs,n_dec);
    LIQUID_VLA(unsigned char, msg_org, n_dec);   // original message
    LIQUID_VLA(unsigned char, msg_enc_0, n_enc); // encoded message (orig)
    LIQUID_VLA(unsigned char, msg_enc_1, n_enc); // encoded message (copy)
    LIQUID_VLA(unsigned char, msg_dec_0, n_dec); // decoded message (orig)
    LIQUID_VLA(unsigned char, msg_dec_1, n_dec); // decoded message (copy)

    // initialize random message bits
    unsigned int i;
    for (i=0; i<n_dec; i++)
        msg_org[i] = rand() & 0xff;

    // encode message
    fec_encode(q0,n_dec,msg_org,msg_enc_0);

    // copy object and repeat
    fec q1 = fec_copy(q0);
    fec_encode(q1,n_dec,msg_org,msg_enc_1);

    // validate output
    LIQUID_CHECK_ARRAY(msg_enc_0,msg_enc_1,n_enc);

    // initialize random bits for decoding
    for (i=0; i<n_enc; i++) {
        msg_enc_0[i] = rand() & 0xff;
        msg_enc_1[i] = msg_enc_0[i];
    }

    // decode messages and compare
    fec_decode(q0,n_dec,msg_enc_0,msg_dec_0);
    fec_decode(q1,n_dec,msg_enc_1,msg_dec_1);
    LIQUID_CHECK_ARRAY(msg_dec_0,msg_dec_1,n_dec);

    // clean up objects
    fec_destroy(q0);
    fec_destroy(q1);
}

// repeat codes
LIQUID_AUTOTEST(fec_copy_r3,"","",0.1)      { testbench_fec_copy(__q__, LIQUID_FEC_REP3          ); }
LIQUID_AUTOTEST(fec_copy_r5,"","",0.1)      { testbench_fec_copy(__q__, LIQUID_FEC_REP5          ); }

// Hamming block codes
LIQUID_AUTOTEST(fec_copy_h74,"","",0.1)     { testbench_fec_copy(__q__, LIQUID_FEC_HAMMING74     ); }
LIQUID_AUTOTEST(fec_copy_h84,"","",0.1)     { testbench_fec_copy(__q__, LIQUID_FEC_HAMMING84     ); }
LIQUID_AUTOTEST(fec_copy_h128,"","",0.1)    { testbench_fec_copy(__q__, LIQUID_FEC_HAMMING128    ); }

// Golay block codes
LIQUID_AUTOTEST(fec_copy_g2412,"","",0.1)   { testbench_fec_copy(__q__, LIQUID_FEC_GOLAY2412     ); }

// SEC-DED block codecs
LIQUID_AUTOTEST(fec_copy_secded2216,"","",0.1) { testbench_fec_copy(__q__, LIQUID_FEC_SECDED2216 ); }
LIQUID_AUTOTEST(fec_copy_secded3932,"","",0.1) { testbench_fec_copy(__q__, LIQUID_FEC_SECDED3932 ); }
LIQUID_AUTOTEST(fec_copy_secded7264,"","",0.1) { testbench_fec_copy(__q__, LIQUID_FEC_SECDED7264 ); }

// convolutional codes
LIQUID_AUTOTEST(fec_copy_v27,"","",0.1)     { testbench_fec_copy(__q__, LIQUID_FEC_CONV_V27      ); }
LIQUID_AUTOTEST(fec_copy_v29,"","",0.1)     { testbench_fec_copy(__q__, LIQUID_FEC_CONV_V29      ); }
LIQUID_AUTOTEST(fec_copy_v39,"","",0.1)     { testbench_fec_copy(__q__, LIQUID_FEC_CONV_V39      ); }
LIQUID_AUTOTEST(fec_copy_v615,"","",0.1)    { testbench_fec_copy(__q__, LIQUID_FEC_CONV_V615     ); }

// convolutional codes (punctured)
LIQUID_AUTOTEST(fec_copy_v27p23,"","",0.1)  { testbench_fec_copy(__q__, LIQUID_FEC_CONV_V27P23   ); }
LIQUID_AUTOTEST(fec_copy_v27p34,"","",0.1)  { testbench_fec_copy(__q__, LIQUID_FEC_CONV_V27P34   ); }
LIQUID_AUTOTEST(fec_copy_v27p45,"","",0.1)  { testbench_fec_copy(__q__, LIQUID_FEC_CONV_V27P45   ); }
LIQUID_AUTOTEST(fec_copy_v27p56,"","",0.1)  { testbench_fec_copy(__q__, LIQUID_FEC_CONV_V27P56   ); }
LIQUID_AUTOTEST(fec_copy_v27p67,"","",0.1)  { testbench_fec_copy(__q__, LIQUID_FEC_CONV_V27P67   ); }
LIQUID_AUTOTEST(fec_copy_v27p78,"","",0.1)  { testbench_fec_copy(__q__, LIQUID_FEC_CONV_V27P78   ); }

LIQUID_AUTOTEST(fec_copy_v29p23,"","",0.1)  { testbench_fec_copy(__q__, LIQUID_FEC_CONV_V29P23   ); }
LIQUID_AUTOTEST(fec_copy_v29p34,"","",0.1)  { testbench_fec_copy(__q__, LIQUID_FEC_CONV_V29P34   ); }
LIQUID_AUTOTEST(fec_copy_v29p45,"","",0.1)  { testbench_fec_copy(__q__, LIQUID_FEC_CONV_V29P45   ); }
LIQUID_AUTOTEST(fec_copy_v29p56,"","",0.1)  { testbench_fec_copy(__q__, LIQUID_FEC_CONV_V29P56   ); }
LIQUID_AUTOTEST(fec_copy_v29p67,"","",0.1)  { testbench_fec_copy(__q__, LIQUID_FEC_CONV_V29P67   ); }
LIQUID_AUTOTEST(fec_copy_v29p78,"","",0.1)  { testbench_fec_copy(__q__, LIQUID_FEC_CONV_V29P78   ); }

// Reed-Solomon block codes
LIQUID_AUTOTEST(fec_copy_rs8,"","",0.1)     { testbench_fec_copy(__q__, LIQUID_FEC_RS_M8         ); }

