/*
 * Copyright (c) 2007 - 2023 Joseph Gaeddert
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

#include "autotest/autotest.h"
#include "liquid.h"

//
// AUTOTEST: repeat/3 codec
//
void autotest_rep5_codec()
{
    unsigned int n=4;
    unsigned char msg[] = {0x25, 0x62, 0x3F, 0x52};
    fec_scheme fs = LIQUID_FEC_REP5;

    // create arrays
    unsigned int n_enc = fec_get_enc_msg_length(fs,n);
    unsigned char msg_dec[n];
    unsigned char msg_enc[n_enc];

    // create object
    fec q = fec_create(fs,NULL);

    // encode message
    fec_encode(q, n, msg, msg_enc);
    
    // corrupt encoded message, but no so much that it
    // can't be decoded
    msg_enc[ 0] = ~msg_enc[ 0];
    msg_enc[ 4] = ~msg_enc[ 4];
//  msg_enc[ 8] = ~msg_enc[ 8];
//  msg_enc[12] = ~msg_enc[12];
//  msg_enc[16] = ~msg_enc[16];

    msg_enc[ 1] = ~msg_enc[ 1];
//  msg_enc[ 5] = ~msg_enc[ 5];
    msg_enc[ 9] = ~msg_enc[ 9];
//  msg_enc[13] = ~msg_enc[13];
//  msg_enc[17] = ~msg_enc[17];

//  msg_enc[ 2] = ~msg_enc[ 2];
//  msg_enc[ 6] = ~msg_enc[ 6];
    msg_enc[10] = ~msg_enc[10];
    msg_enc[14] = ~msg_enc[14];
//  msg_enc[18] = ~msg_enc[18];

    msg_enc[ 3] = ~msg_enc[ 3];
//  msg_enc[ 7] = ~msg_enc[ 7];
//  msg_enc[11] = ~msg_enc[11];
//  msg_enc[15] = ~msg_enc[15];
    msg_enc[19] = ~msg_enc[19];

    // decode message
    fec_decode(q, n, msg_enc, msg_dec);

    // validate data are the same
    CONTEND_SAME_DATA(msg, msg_dec, n);

    // clean up objects
    fec_destroy(q);
}

