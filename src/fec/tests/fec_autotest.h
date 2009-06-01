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

#ifndef __LIQUID_FEC_AUTOTEST_H__
#define __LIQUID_FEC_AUTOTEST_H__

#include <stdlib.h>

#include "autotest/autotest.h"
#include "liquid.h"

// Helper function to keep code base small
void fec_test_codec(fec_scheme _fs, unsigned int _n, void * _opts)
{
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

    // channel: add error(s)
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
void autotest_rep3_generic()        { fec_test_codec(FEC_REP3,64,NULL);      }
void autotest_hamming74_generic()   { fec_test_codec(FEC_HAMMING74,64,NULL); }

#endif 

