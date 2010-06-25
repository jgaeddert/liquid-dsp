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
// 2/3-rate (8,12) Hamming code
//
//  bit position    1   2   3   4   5   6   7   8   9   10  11  12
//  encoded bits    P1  P2  1   P4  2   3   4   P8  5   6   7   8
//
//  parity bit  P1  x   .   x   .   x   .   x   .   x   .   x   .
//  coveratge   P2  .   x   x   .   .   x   x   .   .   x   x   .
//              P4  .   .   .   x   x   x   x   .   .   .   .   x
//              P8  .   .   .   .   .   .   .   x   x   x   x   x

#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

// parity bit coverage mask (collapsed version of figure above, stripping
// out parity bits P1, P2, P4, P8 and only including data bits 1:8)
//
// bit position     3   5   6   7   9   10  11  12
//
//  parity bit  P1  x   x   .   x   x   .   x   .   =   1101 1010
//  coverage    P2  x   .   x   x   .   x   x   .   =   1011 0110
//              P4  .   x   x   x   .   .   .   x   =   0111 0001
//              P8  .   .   .   .   x   x   x   x   =   0000 1111
#define HAMMING128_M1   0xda    // 1101 1010
#define HAMMING128_M2   0xb6    // 1011 0110
#define HAMMING128_M4   0x71    // 0111 0001
#define HAMMING128_M8   0x0f    // 0000 1111

#if 0
// syndrome bit mask (same as a figure XXX but with self-parity bits
// disabled)
//
//  bit position    1   2   3   4   5   6   7   8   9   10  11  12
//  encoded bits    P1  P2  1   P4  2   3   4   P8  5   6   7   8
//
//  parity bit  P1  .   .   x   .   x   .   x   .   x   .   x   .   =   0010 1010 1010
//  coveratge   P2  .   .   x   .   .   x   x   .   .   x   x   .   =   0010 0110 0110
//              P4  .   .   .   .   x   x   x   .   .   .   .   x   =   0000 1110 0001
//              P8  .   .   .   .   .   .   .   .   x   x   x   x   =   0000 0000 1111
#define HAMMING128_S1   0x02aa  // .... 0010 1010 1010
#define HAMMING128_S2   0x0266  // .... 0010 0110 0110
#define HAMMING128_S4   0x00e1  // .... 0000 1110 0001
#define HAMMING128_S8   0x000f  // .... 0000 0000 1111

#else
/*
#define HAMMING128_S1   0x0555  // .... 0101 0101 0101
#define HAMMING128_S2   0x0333  // .... 0011 0011 0011
#define HAMMING128_S4   0x0f0f  // .... 1111 0000 1111
#define HAMMING128_S8   0x00ff  // .... 0000 1111 1111
*/

#define HAMMING128_S1   0x0aaa  // .... 1010 1010 1010
#define HAMMING128_S2   0x0666  // .... 0110 0110 0110
#define HAMMING128_S4   0x01e1  // .... 0001 1110 0001
#define HAMMING128_S8   0x001f  // .... 0000 0001 1111

#endif

// decode Hamming(12,8) symbol by simply stripping out
// original data bits: symbol:  [..x. xxx. xxxx]
//                               0000 0000 1111     0x000f
//                               0000 1110 0000     0x00e0
//                               0010 0000 0000     0x0200
#define fec_hamming128_decode_symbol(_s)    \
    ( ((_s & 0x000f)     )   |              \
      ((_s & 0x00e0) >> 1)   |              \
      ((_s & 0x0200) >> 2)                  \
    )

// binary dot-product (count ones modulo 2)
#define bdotprod(x,y) (count_ones_static((x)&(y)) & 0x0001)

unsigned int fec_hamming128_encode(unsigned int _sym_dec)
{
    // validate input
    if (_sym_dec >= (1<<8)) {
        fprintf(stderr,"error, fec_hamming128_encode(), input symbol too large\n");
        exit(1);
    }

    // compute parity bits
    unsigned int p1 = bdotprod(_sym_dec, HAMMING128_M1);
    unsigned int p2 = bdotprod(_sym_dec, HAMMING128_M2);
    unsigned int p4 = bdotprod(_sym_dec, HAMMING128_M4);
    unsigned int p8 = bdotprod(_sym_dec, HAMMING128_M8);

    printf("parity bits (p1,p2,p4,p8) : (%1u,%1u,%1u,%1u)\n", p1, p2, p4, p8);

    // encode symbol    [..x. xxx. xxxx]
    unsigned int sym_enc = ((_sym_dec & 0x000f) << 0) |
                           ((_sym_dec & 0x0070) << 1) |
                           ((_sym_dec & 0x0080) << 2) |
                           ( p1 << 11 ) |
                           ( p2 << 10 ) |
                           ( p4 << 8  ) |
                           ( p8 << 4  );

    return sym_enc;
}

unsigned int fec_hamming128_decode(unsigned int _sym_enc)
{
    // validate input
    if (_sym_enc >= (1<<12)) {
        fprintf(stderr,"error, fec_hamming128_decode(), input symbol too large\n");
        exit(1);
    }

    // compute syndrome bits
    unsigned int s1 = bdotprod(_sym_enc, HAMMING128_S1);
    unsigned int s2 = bdotprod(_sym_enc, HAMMING128_S2);
    unsigned int s4 = bdotprod(_sym_enc, HAMMING128_S4);
    unsigned int s8 = bdotprod(_sym_enc, HAMMING128_S8);

    printf("syndrome bits (s1,s2,s4,s8) : (%1u,%1u,%1u,%1u)\n", s1, s2, s4, s8);

    // index
    unsigned int z = (s8<<3) | (s4<<2) | (s2<<1) | s1;
    printf("syndrome z : %u\n", z);

    // flip bit at this position
    if (z) {
        printf("error detected!\n");
        _sym_enc ^= 1 << (12-z);
    }

    // strip data bits from encoded symbol
    unsigned int sym_dec = fec_hamming128_decode_symbol(_sym_enc);

    return sym_dec;
}

#if 0
// create Hamming(12,8) codec object
fec fec_hamming128_create(void * _opts)
{
    fec q = (fec) malloc(sizeof(struct fec_s));

    // set scheme
    q->scheme = FEC_hamming128;
    q->rate = fec_get_rate(q->scheme);

    // set internal function pointers
    q->encode_func = &fec_hamming128_encode;
    q->decode_func = &fec_hamming128_decode;

    return q;
}

// destroy Hamming(12,8) object
void fec_hamming128_destroy(fec _q)
{
    free(_q);
}

// encode block of data using Hamming(12,8) encoder
//
//  _q              :   encoder/decoder object
//  _dec_msg_len    :   decoded message length (number of bytes)
//  _msg_dec        :   decoded message [size: 1 x _dec_msg_len]
//  _msg_enc        :   encoded message [size: 1 x 2*_dec_msg_len]
void fec_hamming128_encode(fec _q,
                          unsigned int _dec_msg_len,
                          unsigned char *_msg_dec,
                          unsigned char *_msg_enc)
{
    unsigned int i, j=0;
    unsigned char s0, s1;
    for (i=0; i<_dec_msg_len; i++) {
        s0 = (_msg_dec[i] >> 4) & 0x0f;
        s1 = (_msg_dec[i] >> 0) & 0x0f;
        _msg_enc[j+0] = hamming128_enc[s0];
        _msg_enc[j+1] = hamming128_enc[s1];
        j+=2;
    }
}

// decode block of data using Hamming(12,8) decoder
//
//  _q              :   encoder/decoder object
//  _dec_msg_len    :   decoded message length (number of bytes)
//  _msg_enc        :   encoded message [size: 1 x 2*_dec_msg_len]
//  _msg_dec        :   decoded message [size: 1 x _dec_msg_len]
//
//unsigned int
void fec_hamming128_decode(fec _q,
                          unsigned int _dec_msg_len,
                          unsigned char *_msg_enc,
                          unsigned char *_msg_dec)
{
    unsigned int i, num_errors=0;
    unsigned char r0, r1, z0, z1, s0, s1;
    for (i=0; i<_dec_msg_len; i++) {
        r0 = _msg_enc[2*i+0];
        r1 = _msg_enc[2*i+1];

        //printf("%u :\n", i);

        // compute syndromes
        z0 = fec_hamming128_compute_syndrome(r0);
        z1 = fec_hamming128_compute_syndrome(r1);

        //printf("  syndrome[%u]          : %d, %d\n", i, (int)z0, (int)z1);
        //printf("  input symbols[%u]     : 0x%.2x, 0x%.2x\n", i, r0, r1);

        if (z0) r0 ^= hamming128_bflip[z0];
        if (z1) r1 ^= hamming128_bflip[z1];

        num_errors += (z0) ? 1 : 0;
        num_errors += (z1) ? 1 : 0;

        //printf("  corrected symbols[%u] : 0x%.2x, 0x%.2x\n", i, r0, r1);

        s0 = fec_hamming128_decode_symbol(r0);
        s1 = fec_hamming128_decode_symbol(r1);

        //printf("  decoded symbols[%u]   : 0x%.1x%.1x\n", i, s0, s1);

        _msg_dec[i] = (s0 << 4) | s1;
    }
    //return num_errors;
}
#endif


