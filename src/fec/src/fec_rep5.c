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

//
// FEC, repeat code
// 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "liquid.internal.h"

// create rep5 codec object
fec fec_rep5_create(void * _opts)
{
    fec q = (fec) malloc(sizeof(struct fec_s));

    q->scheme = LIQUID_FEC_REP5;
    q->rate = fec_get_rate(q->scheme);

    q->encode_func      = &fec_rep5_encode;
    q->decode_func      = &fec_rep5_decode;
    q->decode_soft_func = &fec_rep5_decode_soft;

    return q;
}

// destroy rep5 object
void fec_rep5_destroy(fec _q)
{
    free(_q);
}

// print rep5 object
void fec_rep5_print(fec _q)
{
    printf("fec_rep5 [r: %3.2f]\n", _q->rate);
}

// encode block of data using rep5 encoder
//
//  _q              :   encoder/decoder object
//  _dec_msg_len    :   decoded message length (number of bytes)
//  _msg_dec        :   decoded message [size: 1 x _dec_msg_len]
//  _msg_enc        :   encoded message [size: 1 x 5*_dec_msg_len]
void fec_rep5_encode(fec _q,
                     unsigned int _dec_msg_len,
                     unsigned char *_msg_dec,
                     unsigned char *_msg_enc)
{
    unsigned int i;
    for (i=0; i<5; i++) {
        memcpy(&_msg_enc[i*_dec_msg_len], _msg_dec, _dec_msg_len);
    }
}

// decode block of data using rep5 decoder
//
//  _q              :   encoder/decoder object
//  _dec_msg_len    :   decoded message length (number of bytes)
//  _msg_enc        :   encoded message [size: 1 x 5*_dec_msg_len]
//  _msg_dec        :   decoded message [size: 1 x _dec_msg_len]
void fec_rep5_decode(fec _q,
                     unsigned int _dec_msg_len,
                     unsigned char *_msg_enc,
                     unsigned char *_msg_dec)
{
    unsigned char s0, s1, s2, s3, s4;
    unsigned int i, num_errors=0;
    for (i=0; i<_dec_msg_len; i++) {
        s0 = _msg_enc[i                 ];
        s1 = _msg_enc[i +   _dec_msg_len];
        s2 = _msg_enc[i + 2*_dec_msg_len];
        s3 = _msg_enc[i + 3*_dec_msg_len];
        s4 = _msg_enc[i + 4*_dec_msg_len];

        // compute all triplet combinations
        _msg_dec[i] =   (s0 & s1 & s2) |
                        (s0 & s1 & s3) |
                        (s0 & s1 & s4) |
                        (s0 & s2 & s3) |
                        (s0 & s2 & s4) |
                        (s0 & s3 & s4) |
                        (s1 & s2 & s3) |
                        (s1 & s2 & s4) |
                        (s1 & s3 & s4) |
                        (s2 & s3 & s4);
    
        //num_errors += (s0 ^ s1) | (s0 ^ s2) | (s1 ^ s2) ? 1 : 0;
        num_errors += 0;
    }
    //return num_errors;
}

// decode block of data using rep5 decoder (soft metrics)
//
//  _q              :   encoder/decoder object
//  _dec_msg_len    :   decoded message length (number of bytes)
//  _msg_enc        :   encoded message [size: 1 x 5*_dec_msg_len]
//  _msg_dec        :   decoded message [size: 1 x _dec_msg_len]
void fec_rep5_decode_soft(fec _q,
                          unsigned int _dec_msg_len,
                          unsigned char * _msg_enc,
                          unsigned char * _msg_dec)
{
    unsigned char s0, s1, s2, s3, s4;
    unsigned int i;
    unsigned int j;
    unsigned int s_hat;
    //unsigned int num_errors=0;
    for (i=0; i<_dec_msg_len; i++) {

        // clear decoded message
        _msg_dec[i] = 0x00;

        for (j=0; j<8; j++) {
            s0 = _msg_enc[8*i                    + j];
            s1 = _msg_enc[8*(i +   _dec_msg_len) + j];
            s2 = _msg_enc[8*(i + 2*_dec_msg_len) + j];
            s3 = _msg_enc[8*(i + 3*_dec_msg_len) + j];
            s4 = _msg_enc[8*(i + 4*_dec_msg_len) + j];

            // average three symbols and make decision
            s_hat = (s0 + s1 + s2 + s3 + s4)/5;

            _msg_dec[i] |= (s_hat > LIQUID_SOFTBIT_ERASURE) ? (1 << (8-j-1)) : 0x00;
            
        }

    }
}
