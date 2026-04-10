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
void testbench_fec_codec(liquid_autotest __q__, fec_scheme _fs, unsigned int _n, void * _opts)
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

    // generate fec object
    fec q = fec_create(_fs,_opts);

    // create arrays
    unsigned int n_enc = fec_get_enc_msg_length(_fs,_n);
    LIQUID_VLA(unsigned char, msg, _n);          // original message
    LIQUID_VLA(unsigned char, msg_enc, n_enc);   // encoded message
    LIQUID_VLA(unsigned char, msg_dec, _n);      // decoded message

    // initialize message
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
    LIQUID_CHECK_ARRAY(msg,msg_dec,_n);

    // clean up objects
    fec_destroy(q);
}

// 
// AUTOTESTS: basic encode/decode functionality
//

// repeat codes
LIQUID_AUTOTEST(fec_r3,"","",0.1)      { testbench_fec_codec(__q__, LIQUID_FEC_REP3,          64, NULL); }
LIQUID_AUTOTEST(fec_r5,"","",0.1)      { testbench_fec_codec(__q__, LIQUID_FEC_REP5,          64, NULL); }

// Hamming block codes
LIQUID_AUTOTEST(fec_h74,"","",0.1)     { testbench_fec_codec(__q__, LIQUID_FEC_HAMMING74,     64, NULL); }
LIQUID_AUTOTEST(fec_h84,"","",0.1)     { testbench_fec_codec(__q__, LIQUID_FEC_HAMMING84,     64, NULL); }
LIQUID_AUTOTEST(fec_h128,"","",0.1)    { testbench_fec_codec(__q__, LIQUID_FEC_HAMMING128,    64, NULL); }

// Golay block codes
LIQUID_AUTOTEST(fec_g2412,"","",0.1)   { testbench_fec_codec(__q__, LIQUID_FEC_GOLAY2412,     64, NULL); }

// SEC-DED block codecs
LIQUID_AUTOTEST(fec_secded2216,"","",0.1) { testbench_fec_codec(__q__, LIQUID_FEC_SECDED2216, 64, NULL); }
LIQUID_AUTOTEST(fec_secded3932,"","",0.1) { testbench_fec_codec(__q__, LIQUID_FEC_SECDED3932, 64, NULL); }
LIQUID_AUTOTEST(fec_secded7264,"","",0.1) { testbench_fec_codec(__q__, LIQUID_FEC_SECDED7264, 64, NULL); }

// convolutional codes
LIQUID_AUTOTEST(fec_v27,"","",0.1)     { testbench_fec_codec(__q__, LIQUID_FEC_CONV_V27,      64, NULL); }
LIQUID_AUTOTEST(fec_v29,"","",0.1)     { testbench_fec_codec(__q__, LIQUID_FEC_CONV_V29,      64, NULL); }
LIQUID_AUTOTEST(fec_v39,"","",0.1)     { testbench_fec_codec(__q__, LIQUID_FEC_CONV_V39,      64, NULL); }
LIQUID_AUTOTEST(fec_v615,"","",0.1)    { testbench_fec_codec(__q__, LIQUID_FEC_CONV_V615,     64, NULL); }

// convolutional codes (punctured)
LIQUID_AUTOTEST(fec_v27p23,"","",0.1)  { testbench_fec_codec(__q__, LIQUID_FEC_CONV_V27P23,   64, NULL); }
LIQUID_AUTOTEST(fec_v27p34,"","",0.1)  { testbench_fec_codec(__q__, LIQUID_FEC_CONV_V27P34,   64, NULL); }
LIQUID_AUTOTEST(fec_v27p45,"","",0.1)  { testbench_fec_codec(__q__, LIQUID_FEC_CONV_V27P45,   64, NULL); }
LIQUID_AUTOTEST(fec_v27p56,"","",0.1)  { testbench_fec_codec(__q__, LIQUID_FEC_CONV_V27P56,   64, NULL); }
LIQUID_AUTOTEST(fec_v27p67,"","",0.1)  { testbench_fec_codec(__q__, LIQUID_FEC_CONV_V27P67,   64, NULL); }
LIQUID_AUTOTEST(fec_v27p78,"","",0.1)  { testbench_fec_codec(__q__, LIQUID_FEC_CONV_V27P78,   64, NULL); }

LIQUID_AUTOTEST(fec_v29p23,"","",0.1)  { testbench_fec_codec(__q__, LIQUID_FEC_CONV_V29P23,   64, NULL); }
LIQUID_AUTOTEST(fec_v29p34,"","",0.1)  { testbench_fec_codec(__q__, LIQUID_FEC_CONV_V29P34,   64, NULL); }
LIQUID_AUTOTEST(fec_v29p45,"","",0.1)  { testbench_fec_codec(__q__, LIQUID_FEC_CONV_V29P45,   64, NULL); }
LIQUID_AUTOTEST(fec_v29p56,"","",0.1)  { testbench_fec_codec(__q__, LIQUID_FEC_CONV_V29P56,   64, NULL); }
LIQUID_AUTOTEST(fec_v29p67,"","",0.1)  { testbench_fec_codec(__q__, LIQUID_FEC_CONV_V29P67,   64, NULL); }
LIQUID_AUTOTEST(fec_v29p78,"","",0.1)  { testbench_fec_codec(__q__, LIQUID_FEC_CONV_V29P78,   64, NULL); }

// Reed-Solomon block codes
LIQUID_AUTOTEST(fec_rs8,"","",0.1)     { testbench_fec_codec(__q__, LIQUID_FEC_RS_M8,         64, NULL); }


