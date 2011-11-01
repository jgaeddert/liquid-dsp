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
// SEC-DED (22,16) 8/11-rate forward error-correction block code
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

#define DEBUG_FEC_SECDED2216 0

// P matrix [6 x 16 bits], [6 x 2 bytes]
//  1001 1001 0011 1100 :
//  0011 1110 1000 1010 :
//  1110 1110 0110 0000 :
//  1110 0001 1101 0001 :
//  0001 0011 1100 0111 :
//  0100 0100 0011 1111 :
unsigned char secded2216_P[12] = {
    0x99, 0x3c,
    0x3e, 0x8a,
    0xee, 0x60,
    0xe1, 0xd1,
    0x13, 0xc7,
    0x44, 0x3f};

// syndrome vectors for errors of weight 1
unsigned char secded2216_syndrome_w1[22] = {
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0};

// compute parity on 16-bit input
unsigned char fec_secded2216_compute_parity(unsigned char * _m)
{
    // compute encoded/transmitted message: v = m*G
    unsigned char parity = 0x00;

    // TODO : unwrap this loop
    unsigned int i;
    for (i=0; i<6; i++) {
        parity <<= 1;

        unsigned int p = liquid_c_ones[ secded2216_P[2*i+0] & _m[0] ] +
                         liquid_c_ones[ secded2216_P[2*i+1] & _m[1] ];

        parity |= p & 0x01;
    }

    return parity;
}

// compute syndrome on 22-bit input
unsigned char fec_secded2216_compute_syndrome(unsigned char * _v)
{
    // TODO : unwrap this loop
    unsigned int i;
    unsigned char syndrome = 0x00;
    for (i=0; i<6; i++) {
        syndrome <<= 1;

        unsigned int p =
            ( (_v[0] & (1<<(6-i-1))) ? 1 : 0 )+
            liquid_c_ones[ secded2216_P[2*i+0] & _v[1] ] +
            liquid_c_ones[ secded2216_P[2*i+1] & _v[2] ];

        syndrome |= p & 0x01;
    }

    return syndrome;
}

// compute encoded/transmitted message: v = m*G
void fec_secded2216_encode_symbol(unsigned char * _sym_dec,
                                  unsigned char * _sym_enc)
{
    // first six bits is parity block
    _sym_enc[0] = fec_secded2216_compute_parity(_sym_dec);

    // copy last two values
    _sym_enc[1] = _sym_dec[0];
    _sym_enc[2] = _sym_dec[1];
}

void fec_secded2216_decode_symbol(unsigned char * _sym_enc,
                                  unsigned char * _sym_dec)
{
    // validate input
    if (_sym_enc[0] >= (1<<6)) {
        fprintf(stderr,"error, fec_secded2216_decode_symbol(), input symbol too large\n");
        exit(1);
    }

    unsigned int i;

    // state variables
    unsigned char e_hat[3] = {0,0,0};    // estimated error vector

    // compute syndrome vector, s = r*H^T = ( H*r^T )^T
    unsigned char s = fec_secded2216_compute_syndrome(_sym_enc);

    // compute weight of s
    unsigned int ws = liquid_c_ones[s];

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
#if 0
        for (n=0; n<22; n++) {
            if (s == secded2216_syndrome_w1[n]) {
                // set estimated error vector (set bit at appropriate index)
                // TODO : check this...
                div_t d = div(n,8);
                e_hat[2-d.quot] = 1 << d.rem;

                // set flag and break from loop
                syndrome_match = 1;
                break;
            }
        }
#else
        // estimate error location
        unsigned char e_test[3]  = {0x00, 0x0000, 0x0001};
        for (n=0; n<22; n++) {
            if (s == fec_secded2216_compute_syndrome(e_test)) {
                // single error detected
                e_hat[0] = e_test[0];
                e_hat[1] = e_test[1];
                e_hat[2] = e_test[2];

                // set flag and break from loop
                syndrome_match = 1;
                break;
            }
            
            // shift e_test
            e_test[0] = (e_test[0] << 1) | ((e_test[1] & 0x80) ? 1 : 0);
            e_test[1] = (e_test[1] << 1) | ((e_test[2] & 0x80) ? 1 : 0);
            e_test[2] <<= 1;
        }
#endif

#if DEBUG_FEC_SECDED2216
        if (syndrome_match)
            printf("secded2216_decode_symbol(): match found!\n");
        else
            printf("secded2216_decode_symbol(): match found!\n");
#endif
    }

    // compute estimated transmit vector (last 64 bits of encoded message)
    // NOTE: indices take into account first element in _sym_enc and e_hat
    //       arrays holds the parity bits
    _sym_dec[0] = _sym_enc[1] ^ e_hat[1];
    _sym_dec[1] = _sym_enc[2] ^ e_hat[2];
}

// create SEC-DED (72,64) codec object
fec fec_secded2216_create(void * _opts)
{
    fec q = (fec) malloc(sizeof(struct fec_s));

    // set scheme
    q->scheme = LIQUID_FEC_SECDED2216;
    q->rate = fec_get_rate(q->scheme);

    // set internal function pointers
    q->encode_func      = &fec_secded2216_encode;
    q->decode_func      = &fec_secded2216_decode;
    q->decode_soft_func = NULL;

    return q;
}

// destroy SEC-DEC (72,64) object
void fec_secded2216_destroy(fec _q)
{
    free(_q);
}

// encode block of data using SEC-DEC (72,64) encoder
//
//  _q              :   encoder/decoder object
//  _dec_msg_len    :   decoded message length (number of bytes)
//  _msg_dec        :   decoded message [size: 1 x _dec_msg_len]
//  _msg_enc        :   encoded message [size: 1 x 2*_dec_msg_len]
void fec_secded2216_encode(fec _q,
                           unsigned int _dec_msg_len,
                           unsigned char *_msg_dec,
                           unsigned char *_msg_enc)
{
    unsigned int i=0;       // decoded byte counter
    unsigned int j=0;       // encoded byte counter
    unsigned char m[2];     // one 16-bit symbol (decoded)
    unsigned char v[3];     // one 22-bit symbol (encoded)

    // determine remainder of input length / 8
    unsigned int r = _dec_msg_len % 2;

#if 0
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
        fec_secded2216_encode_symbol(m,v);

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
        fec_secded2216_encode_symbol(m,v);
        
#if DEBUG_FEC_SECDED2216
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

    assert( j == fec_get_enc_msg_length(LIQUID_FEC_SECDED2216,_dec_msg_len) );
    assert( i == _dec_msg_len);
#endif
}

// decode block of data using SEC-DEC (72,64) decoder
//
//  _q              :   encoder/decoder object
//  _dec_msg_len    :   decoded message length (number of bytes)
//  _msg_enc        :   encoded message [size: 1 x 2*_dec_msg_len]
//  _msg_dec        :   decoded message [size: 1 x _dec_msg_len]
//
//unsigned int
void fec_secded2216_decode(fec _q,
                           unsigned int _dec_msg_len,
                           unsigned char *_msg_enc,
                           unsigned char *_msg_dec)
{
    unsigned int i=0;       // decoded byte counter
    unsigned int j=0;       // encoded byte counter
    unsigned char v[3];     // one 22-bit encoded symbol
    unsigned char m_hat[2]; // one 16-bit decoded symbol
    
    // determine remainder of input length / 8
    unsigned int r = _dec_msg_len % 2;

#if 0
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
        fec_secded2216_decode_symbol(v,m_hat);

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
        fec_secded2216_decode_symbol(v,m_hat);

#if DEBUG_FEC_SECDED2216
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

    assert( j == fec_get_enc_msg_length(LIQUID_FEC_SECDED2216,_dec_msg_len) );
    assert( i == _dec_msg_len);
#endif
    //return num_errors;
}
