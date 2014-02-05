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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "autotest/autotest.h"
#include "liquid.internal.h"

//
// AUTOTEST: Reed-Solomon codecs
//
void autotest_reedsolomon_223_255()
{
#if !LIBFEC_ENABLED
    printf("warning: Reed-Solomon codes unavailable\n");
    return;
#endif

    unsigned int dec_msg_len = 223;

    // compute and test encoded message length
    unsigned int enc_msg_len = fec_get_enc_msg_length(LIQUID_FEC_RS_M8,dec_msg_len);
    CONTEND_EQUALITY( enc_msg_len, 255 );

    // create arrays
    unsigned char msg_org[dec_msg_len]; // original message
    unsigned char msg_enc[enc_msg_len]; // encoded message
    unsigned char msg_rec[enc_msg_len]; // received message
    unsigned char msg_dec[dec_msg_len]; // decoded message

    // initialize original message
    unsigned int i;
    for (i=0; i<dec_msg_len; i++)
        msg_org[i] = i & 0xff;

    // create object
    fec q = fec_create(LIQUID_FEC_RS_M8,NULL);
    if (liquid_autotest_verbose)
        fec_print(q);

    // encode message
    fec_encode(q, dec_msg_len, msg_org, msg_enc);
    
    // corrupt encoded message; can withstand up to 16 symbol errors
    memmove(msg_rec, msg_enc, enc_msg_len*sizeof(unsigned char));
    for (i=0; i<16; i++)
        msg_rec[i] ^= 0x75;

    // decode message
    fec_decode(q, dec_msg_len, msg_rec, msg_dec);

    // validate data are the same
    CONTEND_SAME_DATA(msg_org, msg_dec, dec_msg_len);

    if (liquid_autotest_verbose) {
        printf("enc   dec\n");
        printf("---   ---\n");
        for (i=0; i<dec_msg_len; i++)
            printf("%3u   %3u\n", msg_org[i], msg_dec[i]);
    }

    // clean up objects
    fec_destroy(q);
}

