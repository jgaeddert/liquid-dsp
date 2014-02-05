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
// 1/2-rate (7,4) Hamming code
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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
    q->scheme = LIQUID_FEC_HAMMING74;
    q->rate = fec_get_rate(q->scheme);

    // set internal function pointers
    q->encode_func      = &fec_hamming74_encode;
    q->decode_func      = &fec_hamming74_decode;
    q->decode_soft_func = &fec_hamming74_decode_soft;

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
//  _msg_dec        :   decoded message [size: _dec_msg_len x 1]
//  _msg_enc        :   encoded message [size: ...]
void fec_hamming74_encode(fec _q,
                          unsigned int _dec_msg_len,
                          unsigned char *_msg_dec,
                          unsigned char *_msg_enc)
{
    unsigned int i;         // input byte counter
    unsigned int k=0;       // array bit index

    // compute encoded message length
    unsigned int enc_msg_len = fec_block_get_enc_msg_len(_dec_msg_len,4,7);

    unsigned char s0, s1;   // decoded symbols
    unsigned char m0, m1;   // encoded symbols

    for (i=0; i<_dec_msg_len; i++) {
        // strip two 4-bit symbols from input byte
        s0 = (_msg_dec[i] >> 4) & 0x0f;
        s1 = (_msg_dec[i] >> 0) & 0x0f;

        // encode two 7-bit symbols
        m0 = hamming74_enc_gentab[s0];
        m1 = hamming74_enc_gentab[s1];

        // pack encoded symbols into output array
        liquid_pack_array(_msg_enc, enc_msg_len, k, 7, m0);
        k += 7;
        liquid_pack_array(_msg_enc, enc_msg_len, k, 7, m1);
        k += 7;

        //printf("  %3u : 0x%.2x > 0x%.2x,  0x%.2x > 0x%.2x (k=%u)\n", i, s0, m0, s1, m1, k);
    }
}

// decode block of data using Hamming(7,4) decoder
//
//  _q              :   encoder/decoder object
//  _dec_msg_len    :   decoded message length (number of bytes)
//  _msg_enc        :   encoded message [size: ...]
//  _msg_dec        :   decoded message [size: _dec_msg_len x 1]
//
//unsigned int
void fec_hamming74_decode(fec _q,
                          unsigned int _dec_msg_len,
                          unsigned char *_msg_enc,
                          unsigned char *_msg_dec)
{
    unsigned int i;
    unsigned int k=0;       // array bit index

    // compute encoded message length
    unsigned int enc_msg_len = fec_block_get_enc_msg_len(_dec_msg_len,4,7);

    unsigned char r0, r1;   // received 7-bit symbols
    unsigned char s0, s1;   // decoded 4-bit symbols

    //unsigned char num_errors=0;
    for (i=0; i<_dec_msg_len; i++) {
        // strip two 7-bit symbols from 
        liquid_unpack_array(_msg_enc, enc_msg_len, k, 7, &r0);
        k += 7;
        liquid_unpack_array(_msg_enc, enc_msg_len, k, 7, &r1);
        k += 7;

        s0 = hamming74_dec_gentab[r0];
        s1 = hamming74_dec_gentab[r1];

        _msg_dec[i] = (s0 << 4) | s1;

        //printf("  %3u : 0x%.2x > 0x%.2x,  0x%.2x > 0x%.2x (k=%u)\n", i, r0, s0, r1, s1, k);
    }
    //return num_errors;
}

// decode block of data using Hamming(7,4) soft decoder
//
//  _q              :   encoder/decoder object
//  _dec_msg_len    :   decoded message length (number of bytes)
//  _msg_enc        :   encoded message [size: 8*_enc_msg_len x 1]
//  _msg_dec        :   decoded message [size: _dec_msg_len x 1]
//
//unsigned int
void fec_hamming74_decode_soft(fec _q,
                               unsigned int _dec_msg_len,
                               unsigned char *_msg_enc,
                               unsigned char *_msg_dec)
{
    unsigned int i;
    unsigned int k=0;       // array bit index

    // compute encoded message length
    unsigned int enc_msg_len = fec_block_get_enc_msg_len(_dec_msg_len,4,7);

    // decoded 4-bit symbols
    unsigned char s0;
    unsigned char s1;

    //unsigned char num_errors=0;
    for (i=0; i<_dec_msg_len; i++) {
        s0 = fecsoft_hamming74_decode(&_msg_enc[k  ]);
        s1 = fecsoft_hamming74_decode(&_msg_enc[k+7]);
        k += 14;

        // pack two 4-bit symbols into one 8-bit byte
        _msg_dec[i] = (s0 << 4) | s1;

        //printf("  %3u : 0x%.2x > 0x%.2x,  0x%.2x > 0x%.2x (k=%u)\n", i, r0, s0, r1, s1, k);
    }
    assert(k == 8*enc_msg_len);
    //return num_errors;
}

// 
// internal methods
//

// soft decoding of one symbol
unsigned char fecsoft_hamming74_decode(unsigned char * _soft_bits)
{
    // find symbol with minimum distance from all 2^4 possible
    unsigned int d;             // distance metric
    unsigned int dmin = 0;      // minimum distance
    unsigned char s_hat = 0;    // estimated transmitted symbol
    unsigned char c;            // encoded symbol

    unsigned char s;
    for (s=0; s<16; s++) {
        // encode symbol
        c = hamming74_enc_gentab[s];

        // compute distance metric
        d = 0;
        d += (c & 0x40) ? 255 - _soft_bits[0] : _soft_bits[0];
        d += (c & 0x20) ? 255 - _soft_bits[1] : _soft_bits[1];
        d += (c & 0x10) ? 255 - _soft_bits[2] : _soft_bits[2];
        d += (c & 0x08) ? 255 - _soft_bits[3] : _soft_bits[3];
        d += (c & 0x04) ? 255 - _soft_bits[4] : _soft_bits[4];
        d += (c & 0x02) ? 255 - _soft_bits[5] : _soft_bits[5];
        d += (c & 0x01) ? 255 - _soft_bits[6] : _soft_bits[6];

        if (d < dmin || s==0) {
            s_hat = s;
            dmin = d;
        }
    }
    return s_hat;
}
