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

#include "autotest/autotest.h"
#include "liquid.internal.h"

//
// AUTOTEST: Hamming (7,4) codec
//
void autotest_hamming74_codec()
{
    unsigned int n=4;
    unsigned char msg[] = {0x25, 0x62, 0x3F, 0x52};
    fec_scheme fs = LIQUID_FEC_HAMMING74;

    // create arrays
    unsigned int n_enc = fec_get_enc_msg_length(fs,n);
    unsigned char msg_dec[n];
    unsigned char msg_enc[n_enc];

    // create object
    fec q = fec_create(fs,NULL);
    if (liquid_autotest_verbose)
        fec_print(q);

    // encode message
    fec_encode(q, n, msg, msg_enc);
    
    // corrupt encoded message
    msg_enc[0] ^= 0x04; // position 5
#if 0
    msg_enc[1] ^= 0x04; //
    msg_enc[2] ^= 0x02; //
    msg_enc[3] ^= 0x01; //
    msg_enc[4] ^= 0x80; //
    msg_enc[5] ^= 0x40; //
    msg_enc[6] ^= 0x20; //
#endif

    // decode message
    fec_decode(q, n, msg_enc, msg_dec);

    // validate data are the same
    CONTEND_SAME_DATA(msg, msg_dec, n);

    // clean up objects
    fec_destroy(q);
}

//
// AUTOTEST: Hamming (7,4) codec (soft decoding)
//
void autotest_hamming74_codec_soft()
{
    // generate each of the 2^4=16 symbols, encode, and decode
    // using soft decoding algorithm
    unsigned char s;            // original 4-bit symbol
    unsigned char c;            // encoded 7-bit symbol
    unsigned char c_soft[7];    // soft bits
    unsigned char s_hat;        // decoded symbol

    for (s=0; s<16; s++) {
        // encode using look-up table
        c = hamming74_enc_gentab[s];

        // expand soft bits
        c_soft[0] = (c & 0x40) ? 255 : 0;
        c_soft[1] = (c & 0x20) ? 255 : 0;
        c_soft[2] = (c & 0x10) ? 255 : 0;
        c_soft[3] = (c & 0x08) ? 255 : 0;
        c_soft[4] = (c & 0x04) ? 255 : 0;
        c_soft[5] = (c & 0x02) ? 255 : 0;
        c_soft[6] = (c & 0x01) ? 255 : 0;

        // decode using internal soft decoding method
        s_hat = fecsoft_hamming74_decode(c_soft);

        // contend that data are the same
        CONTEND_EQUALITY(s, s_hat);
    }
}

