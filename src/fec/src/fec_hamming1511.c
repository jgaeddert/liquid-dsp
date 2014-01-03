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
#include <assert.h>

#include "liquid.internal.h"

#define DEBUG_FEC_HAMMING1511 0

//
// Hamming(15,11) code
//
// parity bit coverage mask for encoder (collapsed version of figure
// above, stripping out parity bits P1, P2, P4, P8 and only including
// data bits 1:11)
//
//  parity bit  P1  x   x   .   x   x   .   x   .   x   .   x   = .110 1101 0101
//  coverage    P2  x   .   x   x   .   x   x   .   .   x   x   = .101 1011 0011 
//              P4  .   x   x   x   .   .   .   x   x   x   x   = .011 1000 1111
//              P8  .   .   .   .   x   x   x   x   x   x   x   = .000 0111 1111

#define HAMMING_M1   0x06d5     // .110 1101 0101
#define HAMMING_M2   0x05b3     // .101 1011 0011
#define HAMMING_M4   0x038f     // .011 1000 1111
#define HAMMING_M8   0x007f     // .000 0111 1111

// parity bit coverage mask for decoder; used to compute syndromes
// for decoding a received message (see first figure, above).
#define HAMMING_S1   0x5555  // .101 0101 0101 0101
#define HAMMING_S2   0x3333  // .011 0011 0011 0011
#define HAMMING_S4   0x0f0f  // .000 1111 0000 1111
#define HAMMING_S8   0x00ff  // .000 0000 1111 1111

unsigned int fec_hamming1511_encode_symbol(unsigned int _sym_dec)
{
    // validate input
    if (_sym_dec >= (1<<11)) {
        fprintf(stderr,"error, fec_hamming_encode(), input symbol too large\n");
        exit(1);
    }

    // compute parity bits
    unsigned int p1 = liquid_bdotprod_uint16(_sym_dec, HAMMING_M1);
    unsigned int p2 = liquid_bdotprod_uint16(_sym_dec, HAMMING_M2);
    unsigned int p4 = liquid_bdotprod_uint16(_sym_dec, HAMMING_M4);
    unsigned int p8 = liquid_bdotprod_uint16(_sym_dec, HAMMING_M8);

#if DEBUG_FEC_HAMMING1511
    printf("parity bits (p1,p2,p4,p8) : (%1u,%1u,%1u,%1u)\n", p1, p2, p4, p8);
#endif

    // encode symbol by inserting parity bits with data bits to
    // make a 15-bit symbol
    unsigned int sym_enc = ((_sym_dec & 0x007f) << 0) |
                           ((_sym_dec & 0x0380) << 1) |
                           ((_sym_dec & 0x0400) << 2) |
                           ( p1 << 14 ) |
                           ( p2 << 13 ) |
                           ( p4 << 11 ) |
                           ( p8 << 7  );

    return sym_enc;
}

unsigned int fec_hamming1511_decode_symbol(unsigned int _sym_enc)
{
    // validate input
    if (_sym_enc >= (1<<15)) {
        fprintf(stderr,"error, fec_hamming_decode(), input symbol too large\n");
        exit(1);
    }

    // compute syndrome bits
    unsigned int s1 = liquid_bdotprod_uint16(_sym_enc, HAMMING_S1);
    unsigned int s2 = liquid_bdotprod_uint16(_sym_enc, HAMMING_S2);
    unsigned int s4 = liquid_bdotprod_uint16(_sym_enc, HAMMING_S4);
    unsigned int s8 = liquid_bdotprod_uint16(_sym_enc, HAMMING_S8);

    // index
    unsigned int z = (s8<<3) | (s4<<2) | (s2<<1) | s1;

#if DEBUG_FEC_HAMMING1511
    printf("syndrome bits (s1,s2,s4,s8) : (%1u,%1u,%1u,%1u)\n", s1, s2, s4, s8);
    printf("syndrome z : %u\n", z);
#endif

    // flip bit at this position
    if (z) {
        if (z > 15) {
            // if this happens there are likely too many errors
            // to correct; just pass without trying to do anything
            fprintf(stderr,"warning, fec_hamming1511_decode_symbol(), syndrome index exceeds maximum\n");
        } else {
            //printf("error detected!\n");
            _sym_enc ^= 1 << (15-z);
        }
    }

    // strip data bits (x) from encoded symbol with parity bits (.)
    //      symbol:  [..x. xxx. xxxx]
    //                0000 0000 1111     >  0x000f
    //                0000 1110 0000     >  0x00e0
    //                0010 0000 0000     >  0x0200
    //
    //      symbol: [ -..x .xxx .xxx xxxx]
    //                -000 0000 0xxx xxxx   > 0x007f
    //                -000 0111 0000 0000   > 0x0700
    //                -001 0000 0000 0000   > 0x1000
    unsigned int sym_dec = ((_sym_enc & 0x007f)     )   |
                           ((_sym_enc & 0x0700) >> 1)   |
                           ((_sym_enc & 0x1000) >> 2);


    return sym_dec;
}

