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
    if (liquid_autotest_verbose)
        fec_print(q);

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

