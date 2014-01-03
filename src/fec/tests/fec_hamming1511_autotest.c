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
// AUTOTEST: Hamming (15,11) codec
//
void autotest_hamming1511_codec()
{
    unsigned int n=11;  //
    unsigned int k=15;  //
    unsigned int i;     // index of bit to corrupt

    for (i=0; i<k; i++) {
        // generate symbol
        unsigned int sym_org = rand() % (1<<n);

        // encoded symbol
        unsigned int sym_enc = fec_hamming1511_encode_symbol(sym_org);

        // received symbol
        unsigned int sym_rec = sym_enc ^ (1<<(k-i-1));

        // decoded symbol
        unsigned int sym_dec = fec_hamming1511_decode_symbol(sym_rec);

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

