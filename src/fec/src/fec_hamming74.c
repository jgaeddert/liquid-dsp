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

//
// 1/2-rate (7,4) Hamming code
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

// encoder look-up table
unsigned char hamming74_enc_gentab[16] = {
    0x00, 0x69, 0x2a, 0x43, 0x4c, 0x25, 0x66, 0x0f,
    0x70, 0x19, 0x5a, 0x33, 0x3c, 0x55, 0x16, 0x7f};

// decoder look-up table
unsigned char hamming74_dec_gentab[128] = {
    0x00, 0x00, 0x00, 0x03, 0x00, 0x05, 0x0e, 0x07,
    0x00, 0x09, 0x02, 0x07, 0x04, 0x07, 0x07, 0x07,
    0x00, 0x09, 0x0e, 0x0b, 0x0e, 0x0d, 0x0e, 0x0e,
    0x09, 0x09, 0x0a, 0x09, 0x0c, 0x09, 0x0e, 0x07,
    0x00, 0x05, 0x02, 0x0b, 0x05, 0x05, 0x06, 0x05,
    0x02, 0x01, 0x02, 0x02, 0x0c, 0x05, 0x02, 0x07,
    0x08, 0x0b, 0x0b, 0x0b, 0x0c, 0x05, 0x0e, 0x0b,
    0x0c, 0x09, 0x02, 0x0b, 0x0c, 0x0c, 0x0c, 0x0f,
    0x00, 0x03, 0x03, 0x03, 0x04, 0x0d, 0x06, 0x03,
    0x04, 0x01, 0x0a, 0x03, 0x04, 0x04, 0x04, 0x07,
    0x08, 0x0d, 0x0a, 0x03, 0x0d, 0x0d, 0x0e, 0x0d,
    0x0a, 0x09, 0x0a, 0x0a, 0x04, 0x0d, 0x0a, 0x0f,
    0x08, 0x01, 0x06, 0x03, 0x06, 0x05, 0x06, 0x06,
    0x01, 0x01, 0x02, 0x01, 0x04, 0x01, 0x06, 0x0f,
    0x08, 0x08, 0x08, 0x0b, 0x08, 0x0d, 0x06, 0x0f,
    0x08, 0x01, 0x0a, 0x0f, 0x0c, 0x0f, 0x0f, 0x0f};

// create Hamming(7,4) codec object
fec fec_hamming74_create(void * _opts)
{
    fec q = (fec) malloc(sizeof(struct fec_s));

    // set scheme
    q->scheme = FEC_HAMMING74;
    q->rate = fec_get_rate(q->scheme);

    // set internal function pointers
    q->encode_func = &fec_hamming74_encode;
    q->decode_func = &fec_hamming74_decode;

    return q;
}

// destroy Hamming(7,4) object
void fec_hamming74_destroy(fec _q)
{
    free(_q);
}

// encode block of data using Hamming(7,4) encoder
//
//  _q              :   encoder/decoder object
//  _dec_msg_len    :   decoded message length (number of bytes)
//  _msg_dec        :   decoded message [size: 1 x _dec_msg_len]
//  _msg_enc        :   encoded message [size: 1 x 2*_dec_msg_len]
void fec_hamming74_encode(fec _q,
                          unsigned int _dec_msg_len,
                          unsigned char *_msg_dec,
                          unsigned char *_msg_enc)
{
    unsigned int i, j=0;
    unsigned char s0, s1;
    for (i=0; i<_dec_msg_len; i++) {
        s0 = (_msg_dec[i] >> 4) & 0x0f;
        s1 = (_msg_dec[i] >> 0) & 0x0f;
        _msg_enc[j+0] = hamming74_enc_gentab[s0];
        _msg_enc[j+1] = hamming74_enc_gentab[s1];
        j+=2;
    }
}

// decode block of data using Hamming(7,4) decoder
//
//  _q              :   encoder/decoder object
//  _dec_msg_len    :   decoded message length (number of bytes)
//  _msg_enc        :   encoded message [size: 1 x 2*_dec_msg_len]
//  _msg_dec        :   decoded message [size: 1 x _dec_msg_len]
//
//unsigned int
void fec_hamming74_decode(fec _q,
                          unsigned int _dec_msg_len,
                          unsigned char *_msg_enc,
                          unsigned char *_msg_dec)
{
    unsigned int i;
    unsigned char r0, r1;   // received 7-bit symbols
    unsigned char s0, s1;   // decoded 4-bit symbols
    //unsigned char num_errors=0;
    for (i=0; i<_dec_msg_len; i++) {
        r0 = _msg_enc[2*i+0] & 0x7f;
        r1 = _msg_enc[2*i+1] & 0x7f;

        s0 = hamming74_dec_gentab[r0];
        s1 = hamming74_dec_gentab[r1];

        _msg_dec[i] = (s0 << 4) | s1;
    }
    //return num_errors;
}

