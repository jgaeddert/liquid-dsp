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

// generate random error vector with 'n' ones;
// maybe not efficient but effective
unsigned int golay2412_generate_error_vector(unsigned int _n)
{
    if (_n > 24) {
        fprintf(stderr,"error: golay2412_generate_error_vector(), cannot generate more than 24 errors\n");
        exit(1);
    }

    unsigned int i;

    unsigned int error_locations[24];
    for (i=0; i<24; i++)
        error_locations[i] = 0;

    unsigned int t=0;
    for (i=0; i<_n; i++) {
        do {
            // generate random error location
            t = rand() % 24;

            // check error location
        } while (error_locations[t]);

        error_locations[t] = 1;
    }

    // generate error vector
    unsigned int e = 0;
    for (i=0; i<24; i++)
        e |= error_locations[i] ? (1 << i) : 0;

    return e;
}

//
// AUTOTEST: Golay(24,12) codec
//
void autotest_golay2412_codec()
{
    unsigned int num_trials=50; // number of symbol trials
    unsigned int num_errors;    // number of errors
    unsigned int i;

    for (num_errors=0; num_errors<=3; num_errors++) {
        for (i=0; i<num_trials; i++) {
            // generate symbol
            unsigned int sym_org = rand() % (1<<12);

            // encoded symbol
            unsigned int sym_enc = fec_golay2412_encode_symbol(sym_org);

            // generate error vector
            unsigned int e = golay2412_generate_error_vector(num_errors);

            // received symbol
            unsigned int sym_rec = sym_enc ^ e;

            // decoded symbol
            unsigned int sym_dec = fec_golay2412_decode_symbol(sym_rec);

#if 0
            printf("error index : %u\n", i);
            // print results
            printf("    sym org     :   "); liquid_print_bitstring(sym_org, n); printf("\n");
            printf("    sym enc     :   "); liquid_print_bitstring(sym_enc, k); printf("\n");
            printf("    sym rec     :   "); liquid_print_bitstring(sym_rec, k); printf("\n");
            printf("    sym dec     :   "); liquid_print_bitstring(sym_dec, n); printf("\n");

            // print number of bit errors
            printf("    bit errors  :   %u\n", count_bit_errors(sym_org, sym_dec));
#endif

            // validate data are the same
            CONTEND_EQUALITY(sym_org, sym_dec);
        }
    }
}

