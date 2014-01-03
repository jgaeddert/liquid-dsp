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
// AUTOTEST: SEC-DEC (22,16) codec (no errors)
//
void autotest_secded2216_codec_e0()
{
    // generate symbol
    unsigned char sym_org[2];
    sym_org[0] = rand() & 0xffff;
    sym_org[1] = rand() & 0xffff;

    // encoded symbol
    unsigned char sym_enc[3];
    fec_secded2216_encode_symbol(sym_org, sym_enc);

    // decoded symbol
    unsigned char sym_dec[2];
    fec_secded2216_decode_symbol(sym_enc, sym_dec);

    // validate data are the same
    CONTEND_EQUALITY(sym_org[0], sym_dec[0]);
    CONTEND_EQUALITY(sym_org[1], sym_dec[1]);
}

//
// AUTOTEST: SEC-DEC (22,16) codec (single error)
//
void autotest_secded2216_codec_e1()
{
    unsigned int k; // error location

    for (k=0; k<22; k++) {
        // generate symbol
        unsigned char sym_org[2];
        sym_org[0] = rand() & 0xffff;
        sym_org[1] = rand() & 0xffff;

        // encoded symbol
        unsigned char sym_enc[3];
        fec_secded2216_encode_symbol(sym_org, sym_enc);

        // generate error vector (single error)
        unsigned char e[3] = {0,0,0};
        div_t d = div(k,8);
        e[3-d.quot-1] = 1 << d.rem;

        // received symbol
        unsigned char sym_rec[3];
        sym_rec[0] = sym_enc[0] ^ e[0];
        sym_rec[1] = sym_enc[1] ^ e[1];
        sym_rec[2] = sym_enc[2] ^ e[2];

        // decoded symbol
        unsigned char sym_dec[2];
        fec_secded2216_decode_symbol(sym_rec, sym_dec);

        // validate data are the same
        CONTEND_EQUALITY(sym_org[0], sym_dec[0]);
        CONTEND_EQUALITY(sym_org[1], sym_dec[1]);
    }
}

//
// AUTOTEST: SEC-DEC (22,16) codec (double error detection)
//
void autotest_secded2216_codec_e2()
{
    // total combinations of double errors: nchoosek(22,2) = 231

    unsigned int j;
    unsigned int k;

    for (j=0; j<21; j++) {
        if (liquid_autotest_verbose)
            printf("***** %2u *****\n", j);
        
        for (k=0; k<22-j-1; k++) {
            // generate symbol
            unsigned char sym_org[2];
            sym_org[0] = rand() & 0xffff;
            sym_org[1] = rand() & 0xffff;

            // encoded symbol
            unsigned char sym_enc[3];
            fec_secded2216_encode_symbol(sym_org, sym_enc);

            // generate error vector (single error)
            unsigned char e[3] = {0,0,0};

            div_t dj = div(j,8);
            e[3-dj.quot-1] |= 1 << dj.rem;

            div_t dk = div(k+j+1,8);
            e[3-dk.quot-1] |= 1 << dk.rem;

            // received symbol
            unsigned char sym_rec[3];
            sym_rec[0] = sym_enc[0] ^ e[0];
            sym_rec[1] = sym_enc[1] ^ e[1];
            sym_rec[2] = sym_enc[2] ^ e[2];

            // decoded symbol
            unsigned char sym_dec[2];
            int syndrome_flag = fec_secded2216_decode_symbol(sym_rec, sym_dec);


            if (liquid_autotest_verbose) {
                // print error vector
                printf("%3u, e = ", k);
                liquid_print_bitstring(e[0], 6);
                liquid_print_bitstring(e[1], 8);
                liquid_print_bitstring(e[2], 8);
                printf(" flag=%2d\n", syndrome_flag);
            }

            // validate syndrome flag is '2'
            CONTEND_EQUALITY(syndrome_flag, 2);
        }
    }
}

