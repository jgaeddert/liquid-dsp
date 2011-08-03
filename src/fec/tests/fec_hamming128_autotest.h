/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
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

#include "autotest/autotest.h"
#include "liquid.h"

//
// AUTOTEST: Hamming (12,8) codec
//
void autotest_hamming128_codec()
{
    unsigned int n=4;
    unsigned char msg_org[] = {0x25, 0x62, 0x3F, 0x52};
    fec_scheme fs = LIQUID_FEC_HAMMING128;

    // create arrays
    unsigned int n_enc = fec_get_enc_msg_length(fs,n);
    unsigned char msg_dec[n];
    unsigned char msg_enc[n_enc];

    // create object
    fec q = fec_create(fs,NULL);
    if (liquid_autotest_verbose)
        fec_print(q);

    // encode message
    fec_encode(q, n, msg_org, msg_enc);
    
    // corrupt encoded message
    msg_enc[0] ^= 0x04;
    msg_enc[4] ^= 0x04;

    // decode message
    fec_decode(q, n, msg_enc, msg_dec);

    // validate data are the same
    CONTEND_SAME_DATA(msg_org, msg_dec, n);

    // clean up objects
    fec_destroy(q);
}

//
// AUTOTEST: Hamming (12,8) codec (soft decoding)
//
void autotest_hamming128_codec_soft()
{
    // generate each of the 2^8=256 symbols, encode, and decode
    // using soft decoding algorithm
    unsigned int s;             // original 8-bit symbol
    unsigned int c;             // encoded 8-bit symbol
    unsigned char c_soft[12];   // soft bits
    unsigned int s_hat;         // decoded symbol

    for (s=0; s<256; s++) {
        // encode using internal method
        c = fec_hamming128_encode_symbol(s);

        // expand soft bits
        c_soft[ 0] = (c & 0x0800) ? 255 : 0;
        c_soft[ 1] = (c & 0x0400) ? 255 : 0;
        c_soft[ 2] = (c & 0x0200) ? 255 : 0;
        c_soft[ 3] = (c & 0x0100) ? 255 : 0;
        c_soft[ 4] = (c & 0x0080) ? 255 : 0;
        c_soft[ 5] = (c & 0x0040) ? 255 : 0;
        c_soft[ 6] = (c & 0x0020) ? 255 : 0;
        c_soft[ 7] = (c & 0x0010) ? 255 : 0;
        c_soft[ 8] = (c & 0x0008) ? 255 : 0;
        c_soft[ 9] = (c & 0x0004) ? 255 : 0;
        c_soft[10] = (c & 0x0002) ? 255 : 0;
        c_soft[11] = (c & 0x0001) ? 255 : 0;

        // decode using internal soft decoding method
        s_hat = fecsoft_hamming128_decode(c_soft);

        // contend that data are the same
        CONTEND_EQUALITY(s, s_hat);
    }
}

