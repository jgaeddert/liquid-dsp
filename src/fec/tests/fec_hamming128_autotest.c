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

#include <stdlib.h>
#include <stdio.h>

#include "autotest/autotest.h"
#include "liquid.internal.h"

//
// AUTOTEST: Hamming (12,8) codec
//
void autotest_hamming128_codec()
{
    unsigned int n=8;   //
    unsigned int k=12;  //
    unsigned int i;     // index of bit to corrupt

    for (i=0; i<k; i++) {
        // generate symbol
        unsigned int sym_org = rand() % (1<<n);

        // encoded symbol
        unsigned int sym_enc = fec_hamming128_encode_symbol(sym_org);

        // received symbol
        unsigned int sym_rec = sym_enc ^ (1<<(k-i-1));

        // decoded symbol
        unsigned int sym_dec = fec_hamming128_decode_symbol(sym_rec);

        if (liquid_autotest_verbose) {
            printf("error index : %u\n", i);
            // print results
            printf("    sym org     :   "); liquid_print_bitstring(sym_org, n); printf("\n");
            printf("    sym enc     :   "); liquid_print_bitstring(sym_enc, k); printf("\n");
            printf("    sym rec     :   "); liquid_print_bitstring(sym_rec, k); printf("\n");
            printf("    sym dec     :   "); liquid_print_bitstring(sym_dec, n); printf("\n");

            // print number of bit errors
            printf("    bit errors  :   %u\n", count_bit_errors(sym_org, sym_dec));
        }

        // validate data are the same
        CONTEND_EQUALITY(sym_org, sym_dec);
    }
}

//
// AUTOTEST: Hamming (12,8) codec (soft decoding)
//
void autotest_hamming128_codec_soft()
{
    // generate each of the 2^8=256 symbols, encode, and decode
    // using soft decoding algorithm
    unsigned int s;             // original 8-bit symbol
    unsigned int c;             // encoded 8-bit symbol
    unsigned char c_soft[12];   // soft bits
    unsigned int s_hat;         // decoded symbol

    for (s=0; s<256; s++) {
        // encode using internal method
        c = fec_hamming128_encode_symbol(s);

        // expand soft bits
        c_soft[ 0] = (c & 0x0800) ? 255 : 0;
        c_soft[ 1] = (c & 0x0400) ? 255 : 0;
        c_soft[ 2] = (c & 0x0200) ? 255 : 0;
        c_soft[ 3] = (c & 0x0100) ? 255 : 0;
        c_soft[ 4] = (c & 0x0080) ? 255 : 0;
        c_soft[ 5] = (c & 0x0040) ? 255 : 0;
        c_soft[ 6] = (c & 0x0020) ? 255 : 0;
        c_soft[ 7] = (c & 0x0010) ? 255 : 0;
        c_soft[ 8] = (c & 0x0008) ? 255 : 0;
        c_soft[ 9] = (c & 0x0004) ? 255 : 0;
        c_soft[10] = (c & 0x0002) ? 255 : 0;
        c_soft[11] = (c & 0x0001) ? 255 : 0;

        // decode using internal soft decoding method
        s_hat = fecsoft_hamming128_decode(c_soft);

        // contend that data are the same
        CONTEND_EQUALITY(s, s_hat);
    }
}

