/*
 * Copyright (c) 2011 Joseph Gaeddert
 * Copyright (c) 2011 Virginia Polytechnic Institute & State University
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
// Golay(24,12) half-rate forward error-correction code
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "liquid.internal.h"

#define DEBUG_FEC_GOLAY2412 0

// P matrix [12 x 12]
unsigned int golay2412_P[12] = {
    0x08ed, 0x01db, 0x03b5, 0x0769,
    0x0ed1, 0x0da3, 0x0b47, 0x068f,
    0x0d1d, 0x0a3b, 0x0477, 0x0ffe};

#if 0
// generator matrix [12 x 24]
unsigned int golay2412_G[12] = {
    0x008ed800, 0x001db400, 0x003b5200, 0x00769100,
    0x00ed1080, 0x00da3040, 0x00b47020, 0x0068f010,
    0x00d1d008, 0x00a3b004, 0x00477002, 0x00ffe001};
#endif

// generator matrix transposed [24 x 12]
unsigned int golay2412_Gt[24] = {
    0x08ed, 0x01db, 0x03b5, 0x0769, 0x0ed1, 0x0da3, 0x0b47, 0x068f, 
    0x0d1d, 0x0a3b, 0x0477, 0x0ffe, 0x0800, 0x0400, 0x0200, 0x0100, 
    0x0080, 0x0040, 0x0020, 0x0010, 0x0008, 0x0004, 0x0002, 0x0001};

// parity check matrix [12 x 24]
unsigned int golay2412_H[12] = {
    0x008008ed, 0x004001db, 0x002003b5, 0x00100769,
    0x00080ed1, 0x00040da3, 0x00020b47, 0x0001068f,
    0x00008d1d, 0x00004a3b, 0x00002477, 0x00001ffe};

unsigned int fec_golay2412_encode_symbol(unsigned int _sym_dec)
{
    // validate input
    if (_sym_dec >= (1<<12)) {
        fprintf(stderr,"error, fec_golay2412_encode_symbol(), input symbol too large\n");
        exit(1);
    }

    // compute encoded/transmitted message: v = m*G
    unsigned int sym_enc = 0;
    unsigned int i;
    for (i=0; i<24; i++) {
        sym_enc <<= 1;
        sym_enc |= liquid_count_ones_mod2(golay2412_Gt[i] & _sym_dec);
    }
    return sym_enc;
}

unsigned int fec_golay2412_decode_symbol(unsigned int _sym_enc)
{
    // validate input
    if (_sym_enc >= (1<<24)) {
        fprintf(stderr,"error, fec_golay2412_decode_symbol(), input symbol too large\n");
        exit(1);
    }

    unsigned int i;

#if 0
    // state variables
    unsigned int s=0;       // syndrome vector
    unsigned int e_hat=0;   // estimated error vector
    unsigned int v_hat=0;   // estimated transmitted message
    unsigned int m_hat=0;   // estimated original message
    
    // compute syndrome vector, s = r*H^T = ( H*r^T )^T
    for (i=0; i<12; i++) {
        s <<= 1;
        s |= liquid_count_ones_mod2( golay2412_H[i] & _sym_enc );
    }
    printf("s (syndrome vector)     :\n");
    liquid_print_bitstring(s,12);

    return m_hat;
#else
    // no correction (strip off last 12 bits)
    unsigned int sym_dec = _sym_enc & ((1<<12)-1);
    return _sym_enc & ((1<<12)-1);
#endif
}

// create Golay(24,12) codec object
fec fec_golay2412_create(void * _opts)
{
    fec q = (fec) malloc(sizeof(struct fec_s));

    // set scheme
    q->scheme = LIQUID_FEC_GOLAY2412;
    q->rate = fec_get_rate(q->scheme);

    // set internal function pointers
    q->encode_func      = &fec_golay2412_encode;
    q->decode_func      = &fec_golay2412_decode;
    q->decode_soft_func = NULL;

    return q;
}

// destroy Golay(24,12) object
void fec_golay2412_destroy(fec _q)
{
    free(_q);
}

// encode block of data using Golay(24,12) encoder
//
//  _q              :   encoder/decoder object
//  _dec_msg_len    :   decoded message length (number of bytes)
//  _msg_dec        :   decoded message [size: 1 x _dec_msg_len]
//  _msg_enc        :   encoded message [size: 1 x 2*_dec_msg_len]
void fec_golay2412_encode(fec _q,
                          unsigned int _dec_msg_len,
                          unsigned char *_msg_dec,
                          unsigned char *_msg_enc)
{
    unsigned int i=0;           // decoded byte counter
    unsigned int j=0;           // encoded byte counter
    unsigned int s0, s1, s2;    // three 8-bit symbols
    unsigned int m0, m1;        // two 12-bit symbols (uncoded)
    unsigned int v0, v1;        // two 24-bit symbols (encoded)

    // determine remainder of input length / 3
    unsigned int r = _dec_msg_len % 3;

    for (i=0; i<_dec_msg_len-r; i+=3) {
        // strip three input bytes (two uncoded symbols)
        s0 = _msg_dec[i+0];
        s1 = _msg_dec[i+1];
        s2 = _msg_dec[i+2];

        // pack into two 12-bit symbols
        m0 = ((s0 << 4) & 0x0ff0) | ((s1 >> 4) & 0x000f);
        m1 = ((s1 << 8) & 0x0f00) | ((s2     ) & 0x00ff);

        // encode each 12-bit symbol into a 24-bit symbol
        v0 = fec_golay2412_encode_symbol(m0);
        v1 = fec_golay2412_encode_symbol(m1);

        // unpack two 24-bit symbols into six 8-bit bytes
        // retaining order of bits in output
        _msg_enc[j+0] = (v0 >> 16) & 0xff;
        _msg_enc[j+1] = (v0 >>  8) & 0xff;
        _msg_enc[j+2] = (v0      ) & 0xff;
        _msg_enc[j+3] = (v1 >> 16) & 0xff;
        _msg_enc[j+4] = (v1 >>  8) & 0xff;
        _msg_enc[j+5] = (v1      ) & 0xff;

        j += 6;
    }

    // if input length isn't divisible by 3, encode last 1 or two bytes
    for (i=_dec_msg_len-r; i<_dec_msg_len; i++) {
        // strip last input symbol
        s0 = _msg_dec[i];

        // extend as 12-bit symbol
        m0 = s0;

        // encode into 24-bit symbol
        v0 = fec_golay2412_encode_symbol(m0);

        // unpack one 24-bit symbol into three 8-bit bytes, and
        // append to output array
        _msg_enc[j+0] = ( v0 >> 16 ) & 0xff;
        _msg_enc[j+1] = ( v0 >>  8 ) & 0xff;
        _msg_enc[j+2] = ( v0       ) & 0xff;

        j += 3;
    }

    assert( j == fec_get_enc_msg_length(LIQUID_FEC_GOLAY2412,_dec_msg_len) );
    assert( i == _dec_msg_len);
}

// decode block of data using Golay(24,12) decoder
//
//  _q              :   encoder/decoder object
//  _dec_msg_len    :   decoded message length (number of bytes)
//  _msg_enc        :   encoded message [size: 1 x 2*_dec_msg_len]
//  _msg_dec        :   decoded message [size: 1 x _dec_msg_len]
//
//unsigned int
void fec_golay2412_decode(fec _q,
                          unsigned int _dec_msg_len,
                          unsigned char *_msg_enc,
                          unsigned char *_msg_dec)
{
    unsigned int i=0;                       // decoded byte counter
    unsigned int j=0;                       // encoded byte counter
    unsigned int r0, r1, r2, r3, r4, r5;    // six 8-bit bytes
    unsigned int v0, v1;                    // two 24-bit encoded symbols
    unsigned int m0_hat, m1_hat;            // two 12-bit decoded symbols
    
    // determine remainder of input length / 3
    unsigned int r = _dec_msg_len % 3;

    for (i=0; i<_dec_msg_len-r; i+=3) {
        // strip six input bytes (two encoded symbols)
        r0 = _msg_enc[j+0];
        r1 = _msg_enc[j+1];
        r2 = _msg_enc[j+2];
        r3 = _msg_enc[j+3];
        r4 = _msg_enc[j+4];
        r5 = _msg_enc[j+5];

        // pack six 8-bit symbols into two 24-bit symbols
        v0 = ((r0 << 16) & 0xff0000) | ((r1 <<  8) & 0x00ff00) | ((r2     ) & 0x0000ff);
        v1 = ((r3 << 16) & 0xff0000) | ((r4 <<  8) & 0x00ff00) | ((r5 << 0) & 0x0000ff);

        // decode each symbol into a 12-bit symbol
        m0_hat = fec_golay2412_decode_symbol(v0);
        m1_hat = fec_golay2412_decode_symbol(v1);

        // unpack two 12-bit symbols into three 8-bit bytes
        _msg_dec[i+0] = ((m0_hat >> 4) & 0xff);
        _msg_dec[i+1] = ((m0_hat << 4) & 0xf0) | ((m1_hat >> 8) & 0x0f);
        _msg_dec[i+2] = ((m1_hat     ) & 0xff);

        j += 6;
    }

    // if input length isn't divisible by 3, decode last 1 or two bytes
    for (i=_dec_msg_len-r; i<_dec_msg_len; i++) {
        // strip last input symbol (three bytes)
        r0 = _msg_enc[j+0];
        r1 = _msg_enc[j+1];
        r2 = _msg_enc[j+2];

        // pack three 8-bit symbols into one 24-bit symbol
        v0 = ((r0 << 16) & 0xff0000) | ((r1 <<  8) & 0x00ff00) | ((r2     ) & 0x0000ff);

        // decode into a 12-bit symbol
        m0_hat = fec_golay2412_decode_symbol(v0);

        // retain last 8 bits of 12-bit symbol
        _msg_dec[i] = m0_hat & 0xff;

        j += 3;
    }

    assert( j== fec_get_enc_msg_length(LIQUID_FEC_GOLAY2412,_dec_msg_len) );
    assert( i == _dec_msg_len);

    //return num_errors;
}

