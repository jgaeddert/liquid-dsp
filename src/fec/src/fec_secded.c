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
// SEC-DED (72,64) 8/9-rate forward error-correction block code
//
// References:
//  [Lin:2004] Lin, Shu and Costello, Daniel L. Jr., "Error Control
//      Coding," Prentice Hall, New Jersey, 2nd edition, 2004.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "liquid.internal.h"

#define DEBUG_FEC_SECDED7264 0

// P matrix [8 x 64]
//  11111111 00001111 00001111 00001100 01101000 10001000 10001000 10000000 : 
//  11110000 11111111 00000000 11110011 01100100 01000100 01000100 01000000 : 
//  00110000 11110000 11111111 00001111 00000010 00100010 00100010 00100110 : 
//  11001111 00000000 11110000 11111111 00000001 00010001 00010001 00010110 : 
//  01101000 10001000 10001000 10000000 11111111 00001111 00000000 11110011 : 
//  01100100 01000100 01000100 01000000 11110000 11111111 00001111 00001100 : 
//  00000010 00100010 00100010 00100110 11001111 00000000 11111111 00001111 : 
//  00000001 00010001 00010001 00010110 00110000 11110000 11110000 11111111 : 
unsigned int secded7264_P[16] = {
    0xFF0F0F0C, 0x68888880,
    0xF0FF00F3, 0x64444440,
    0x30F0FF0F, 0x02222226,
    0xCF00F0FF, 0x01111116,
    0x68888880, 0xFF0F00F3,
    0x64444440, 0xF0FF0F0C,
    0x02222226, 0xCF00FF0F,
    0x01111116, 0x30F0F0FF};

// parity check matrix: H = [I(8) P]
unsigned int secded7264_H[24] = {
    0x80, 0xFF0F0F0C, 0x68888880,
    0x40, 0xF0FF00F3, 0x64444440,
    0x20, 0x30F0FF0F, 0x02222226,
    0x10, 0xCF00F0FF, 0x01111116,
    0x08, 0x68888880, 0xFF0F00F3,
    0x04, 0x64444440, 0xF0FF0F0C,
    0x02, 0x02222226, 0xCF00FF0F,
    0x01, 0x01111116, 0x30F0F0FF};

// syndrome vectors for errors of weight 1
unsigned char secded7264_syndrome_w1[72] = {
    0x0b, 0x3b, 0x37, 0x07, 0x19, 0x29, 0x49, 0x89,
    0x16, 0x26, 0x46, 0x86, 0x13, 0x23, 0x43, 0x83,
    0x1c, 0x2c, 0x4c, 0x8c, 0x15, 0x25, 0x45, 0x85,
    0x1a, 0x2a, 0x4a, 0x8a, 0x0d, 0xcd, 0xce, 0x0e,
    0x70, 0x73, 0xb3, 0xb0, 0x51, 0x52, 0x54, 0x58,
    0xa1, 0xa2, 0xa4, 0xa8, 0x31, 0x32, 0x34, 0x38,
    0xc1, 0xc2, 0xc4, 0xc8, 0x61, 0x62, 0x64, 0x68,
    0x91, 0x92, 0x94, 0x98, 0xe0, 0xec, 0xdc, 0xd0,
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};


void fec_secded7264_encode_symbol(unsigned int * _sym_dec,
                                  unsigned int * _sym_enc)
{
    // compute encoded/transmitted message: v = m*G
    unsigned int i;
    _sym_enc[0] = 0;
    for (i=0; i<8; i++) {
        _sym_enc[0] <<= 1;

        unsigned int p = liquid_count_ones(secded7264_P[2*i+0] & _sym_dec[0]) +
                         liquid_count_ones(secded7264_P[2*i+1] & _sym_dec[1]);
        _sym_enc[0] |= p & 0x01;
    }

    // copy last two values
    _sym_enc[1] = _sym_dec[0];
    _sym_enc[2] = _sym_dec[1];
}

void fec_secded7264_decode_symbol(unsigned int * _sym_enc,
                                  unsigned int * _sym_dec)
{
    // validate input
    if (_sym_enc[0] >= (1<<8)) {
        fprintf(stderr,"error, fec_secded7264_decode_symbol(), input symbol too large\n");
        exit(1);
    }

    unsigned int i;

    // state variables
    unsigned int s;                     // syndrome vector
    unsigned int e_hat[3] = {0,0,0};    // estimated error vector

    // compute syndrome vector, s = r*H^T = ( H*r^T )^T
    s = 0;
    for (i=0; i<8; i++) {
        s <<= 1;
#if 0
        unsigned int p =
            ( (r[0] & (1<<(8-i-1))) ? 1 : 0 )+
            liquid_count_ones(secded7264_P[2*i+0] & r[1]) +
            liquid_count_ones(secded7264_P[2*i+1] & r[2]);
#else
        unsigned int p =
            liquid_count_ones(secded7264_H[3*i+0] & _sym_enc[0]) +
            liquid_count_ones(secded7264_H[3*i+1] & _sym_enc[1]) +
            liquid_count_ones(secded7264_H[3*i+2] & _sym_enc[2]);
#endif
        s |= p & 0x01;
    }

    // compute weight of s
    unsigned int ws = liquid_count_ones(s);

    if (ws == 0) {
        // no errors detected; copy input and return
        _sym_dec[0] = _sym_enc[1];
        _sym_dec[1] = _sym_enc[2];
        return;
    } else {
        // estimate error location; search for syndrome with error
        // vector of weight one
        int syndrome_match = 0;

        unsigned int n;
        for (n=0; n<72; n++) {
            if (s == secded7264_syndrome_w1[n]) {
                // set estimated error vector (set bit at appropriate index)
                div_t d = div(n,32);
                e_hat[2-d.quot] = 1 << d.rem;

                // set flag and break from loop
                syndrome_match = 1;
                break;
            }
        }

#if DEBUG_FEC_SECDED7264
        if (syndrome_match)
            printf("secded7264_decode_symbol(): match found!\n");
        else
            printf("secded7264_decode_symbol(): match found!\n");
#endif
    }

    // compute estimated transmit vector (last 64 bits of encoded message)
    // NOTE: indices take into account first element in _sym_enc and e_hat
    //       arrays holds the parity bits
    _sym_dec[0] = _sym_enc[1] ^ e_hat[1];
    _sym_dec[1] = _sym_enc[2] ^ e_hat[2];
}

// create SEC-DED (72,64) codec object
fec fec_secded7264_create(void * _opts)
{
    fec q = (fec) malloc(sizeof(struct fec_s));

    // set scheme
    q->scheme = LIQUID_FEC_SECDED7264;
    q->rate = fec_get_rate(q->scheme);

    // set internal function pointers
    q->encode_func      = &fec_secded7264_encode;
    q->decode_func      = &fec_secded7264_decode;
    q->decode_soft_func = NULL;

    return q;
}

// destroy SEC-DEC (72,64) object
void fec_secded7264_destroy(fec _q)
{
    free(_q);
}

// encode block of data using SEC-DEC (72,64) encoder
//
//  _q              :   encoder/decoder object
//  _dec_msg_len    :   decoded message length (number of bytes)
//  _msg_dec        :   decoded message [size: 1 x _dec_msg_len]
//  _msg_enc        :   encoded message [size: 1 x 2*_dec_msg_len]
void fec_secded7264_encode(fec _q,
                           unsigned int _dec_msg_len,
                           unsigned char *_msg_dec,
                           unsigned char *_msg_enc)
{
    unsigned int i=0;       // decoded byte counter
    unsigned int j=0;       // encoded byte counter
    unsigned int m[2];      // one 64-bit symbol (decoded)
    unsigned int v[3];      // one 72-bit symbol (encoded)

    // determine remainder of input length / 8
    unsigned int r = _dec_msg_len % 8;

    // TODO : devise more efficient way of doing this
    for (i=0; i<_dec_msg_len-r; i+=8) {
        // strip eight input bytes
        m[0] = 0;
        m[0] |= (_msg_dec[i+0] << 24);
        m[0] |= (_msg_dec[i+1] << 16);
        m[0] |= (_msg_dec[i+2] <<  8);
        m[0] |= (_msg_dec[i+3]);

        m[1] = 0;
        m[1] |= (_msg_dec[i+4] << 24);
        m[1] |= (_msg_dec[i+5] << 16);
        m[1] |= (_msg_dec[i+6] <<  8);
        m[1] |= (_msg_dec[i+7]);

        // encode 64-bit symbol into a 72-bit symbol
        fec_secded7264_encode_symbol(m,v);

        // unpack 72-bit symbol into nine 8-bit bytes
        _msg_enc[j+0] = (v[0]      ) & 0xff;
        _msg_enc[j+1] = (v[1] >> 24) & 0xff;
        _msg_enc[j+2] = (v[1] >> 16) & 0xff;
        _msg_enc[j+3] = (v[1] >>  8) & 0xff;
        _msg_enc[j+4] = (v[1]      ) & 0xff;
        _msg_enc[j+5] = (v[2] >> 24) & 0xff;
        _msg_enc[j+6] = (v[2] >> 16) & 0xff;
        _msg_enc[j+7] = (v[2] >>  8) & 0xff;
        _msg_enc[j+8] = (v[2]      ) & 0xff;

        j += 9;
    }

    // if input length isn't divisible by 8, encode last few bytes
    if (r) {
        unsigned int n;
        m[0] = 0;   // clear input message
        m[1] = 0;
        for (n=0; n<r; n++) {
            div_t d = div(n,4); // unsigned int has four 8-bit bytes
            //printf("n = %2u, n/4 = %2u rem %2u\n", n, d.quot, d.rem);
            // strip remaining bytes...
            m[d.quot] |= (_msg_dec[i+n] << (8*(4-d.rem-1)));
        }
        
        // encode 64-bit symbol into a 72-bit symbol
        fec_secded7264_encode_symbol(m,v);
        
#if DEBUG_FEC_SECDED7264
        printf("encoder input:\n");
        printf("  m[0] = %.8x\n", m[0]);
        printf("  m[1] = %.8x\n", m[1]);
        
        printf("encoder output:\n");
        printf("  v[0] =       %.2x\n", v[0]);
        printf("  v[1] = %.8x\n", v[1]);
        printf("  v[2] = %.8x\n", v[2]);
#endif

        // there is no need to actually send all the bytes; the
        // last 8-r bytes are zeros and can be added at the
        // decoder
        _msg_enc[j+0] = v[0] & 0xff;
        for (n=0; n<r; n++)
            _msg_enc[j+n+1] = _msg_dec[i+n];

        i += r;
        j += r+1;
    }

    assert( j == fec_get_enc_msg_length(LIQUID_FEC_SECDED7264,_dec_msg_len) );
    assert( i == _dec_msg_len);
}

// decode block of data using SEC-DEC (72,64) decoder
//
//  _q              :   encoder/decoder object
//  _dec_msg_len    :   decoded message length (number of bytes)
//  _msg_enc        :   encoded message [size: 1 x 2*_dec_msg_len]
//  _msg_dec        :   decoded message [size: 1 x _dec_msg_len]
//
//unsigned int
void fec_secded7264_decode(fec _q,
                           unsigned int _dec_msg_len,
                           unsigned char *_msg_enc,
                           unsigned char *_msg_dec)
{
    unsigned int i=0;       // decoded byte counter
    unsigned int j=0;       // encoded byte counter
    unsigned int v[3];      // two 24-bit encoded symbols
    unsigned int m_hat[2];  //
    
    // determine remainder of input length / 8
    unsigned int r = _dec_msg_len % 8;

    for (i=0; i<_dec_msg_len-r; i+=8) {
        // strip nine input bytes and pack into 72-bit symbol
        v[0] = _msg_enc[j];

        v[1] = 0;
        v[1] |= (_msg_enc[j+1] << 24);
        v[1] |= (_msg_enc[j+2] << 16);
        v[1] |= (_msg_enc[j+3] <<  8);
        v[1] |= (_msg_enc[j+4]);

        v[2] = 0;
        v[2] |= (_msg_enc[j+5] << 24);
        v[2] |= (_msg_enc[j+6] << 16);
        v[2] |= (_msg_enc[j+7] <<  8);
        v[2] |= (_msg_enc[j+8]);

        // decode 72-bit symbol into a 64-bit symbol
        fec_secded7264_decode_symbol(v,m_hat);

        // unpack 64-bit symbol into eight 8-bit bytes
        _msg_dec[i+0] = ((m_hat[0] >> 24) & 0xff);
        _msg_dec[i+1] = ((m_hat[0] >> 16) & 0xff);
        _msg_dec[i+2] = ((m_hat[0] >>  8) & 0xff);
        _msg_dec[i+3] = ((m_hat[0]      ) & 0xff);
        _msg_dec[i+4] = ((m_hat[1] >> 24) & 0xff);
        _msg_dec[i+5] = ((m_hat[1] >> 16) & 0xff);
        _msg_dec[i+6] = ((m_hat[1] >>  8) & 0xff);
        _msg_dec[i+7] = ((m_hat[1]      ) & 0xff);

        j += 9;
    }

    // if input length isn't divisible by 8, decode last several bytes
    if (r) {
        unsigned int n;
        v[0] = 0;
        v[1] = 0;
        v[2] = 0;
        // output length is input + 1 (parity byte)
        for (n=0; n<r+1; n++) {
            div_t d = div(n+3,4); // unsigned int has four 8-bit bytes
            v[d.quot] |= (_msg_enc[j+n] << (8*(4-d.rem-1)));
            //printf("n = %2u, n+3 = %2u, n/4 = %2u rem %2u\n", n, n+3, d.quot, d.rem);
        }

        // decode 72-bit symbol into a 64-bit symbol
        fec_secded7264_decode_symbol(v,m_hat);

#if DEBUG_FEC_SECDED7264
        printf("decoder input:\n");
        printf("  v[0] =       %.2x\n", v[0]);
        printf("  v[1] = %.8x\n", v[1]);
        printf("  v[2] = %.8x\n", v[2]);
        
        printf("decoder output:\n");
        printf("  m[0] = %.8x\n", m_hat[0]);
        printf("  m[1] = %.8x\n", m_hat[1]);
#endif
        
        // pack only relevant bytes
        for (n=0; n<r; n++) {
            div_t d = div(n,4); // unsigned int has four 8-bit bytes
            //printf("n = %2u, n/4 = %2u rem %2u\n", n, d.quot, d.rem);
            // strip remaining bytes...
            _msg_dec[i+n] = (m_hat[d.quot] >> (8*(4-d.rem-1))) & 0xff;
        }

        i += r;
        j += r+1;
    }

    assert( j == fec_get_enc_msg_length(LIQUID_FEC_SECDED7264,_dec_msg_len) );
    assert( i == _dec_msg_len);

    //return num_errors;
}
