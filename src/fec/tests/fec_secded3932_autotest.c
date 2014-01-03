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
// AUTOTEST: SEC-DEC (39,32) codec (no errors)
//
void autotest_secded3932_codec_e0()
{
    // generate symbol
    unsigned char sym_org[4];
    sym_org[0] = rand() & 0xffff;
    sym_org[1] = rand() & 0xffff;
    sym_org[2] = rand() & 0xffff;
    sym_org[3] = rand() & 0xffff;

    // encoded symbol
    unsigned char sym_enc[5];
    fec_secded3932_encode_symbol(sym_org, sym_enc);

    // decoded symbol
    unsigned char sym_dec[4];
    fec_secded3932_decode_symbol(sym_enc, sym_dec);

    // validate data are the same
    CONTEND_EQUALITY(sym_org[0], sym_dec[0]);
    CONTEND_EQUALITY(sym_org[1], sym_dec[1]);
    CONTEND_EQUALITY(sym_org[2], sym_dec[2]);
    CONTEND_EQUALITY(sym_org[3], sym_dec[3]);
}

//
// AUTOTEST: SEC-DEC (39,32) codec (single error)
//
void autotest_secded3932_codec_e1()
{
    unsigned int k; // error location

    for (k=0; k<39; k++) {
        // generate symbol
        unsigned char sym_org[4];
        sym_org[0] = rand() & 0xffff;
        sym_org[1] = rand() & 0xffff;
        sym_org[2] = rand() & 0xffff;
        sym_org[3] = rand() & 0xffff;

        // encoded symbol
        unsigned char sym_enc[5];
        fec_secded3932_encode_symbol(sym_org, sym_enc);

        // generate error vector (single error)
        unsigned char e[5] = {0,0,0,0,0};
        div_t d = div(k,8);
        e[5-d.quot-1] = 1 << d.rem;

        // received symbol
        unsigned char sym_rec[5];
        sym_rec[0] = sym_enc[0] ^ e[0];
        sym_rec[1] = sym_enc[1] ^ e[1];
        sym_rec[2] = sym_enc[2] ^ e[2];
        sym_rec[3] = sym_enc[3] ^ e[3];
        sym_rec[4] = sym_enc[4] ^ e[4];

        // decoded symbol
        unsigned char sym_dec[4];
        fec_secded3932_decode_symbol(sym_rec, sym_dec);

        // validate data are the same
        CONTEND_EQUALITY(sym_org[0], sym_dec[0]);
        CONTEND_EQUALITY(sym_org[1], sym_dec[1]);
        CONTEND_EQUALITY(sym_org[2], sym_dec[2]);
        CONTEND_EQUALITY(sym_org[3], sym_dec[3]);
    }
}

//
// AUTOTEST: SEC-DEC (39,32) codec (double error detection)
//
void autotest_secded3932_codec_e2()
{
    // total combinations of double errors: nchoosek(39,2) = 741

    unsigned int j;
    unsigned int k;

    for (j=0; j<39-1; j++) {
        if (liquid_autotest_verbose)
            printf("***** %2u *****\n", j);
        
        for (k=0; k<39-1-j; k++) {
            // generate symbol
            unsigned char sym_org[4];
            sym_org[0] = rand() & 0xffff;
            sym_org[1] = rand() & 0xffff;
            sym_org[2] = rand() & 0xffff;
            sym_org[3] = rand() & 0xffff;

            // encoded symbol
            unsigned char sym_enc[5];
            fec_secded3932_encode_symbol(sym_org, sym_enc);

            // generate error vector (single error)
            unsigned char e[5] = {0,0,0,0,0};

            div_t dj = div(j,8);
            e[5-dj.quot-1] |= 1 << dj.rem;

            div_t dk = div(k+j+1,8);
            e[5-dk.quot-1] |= 1 << dk.rem;

            // received symbol
            unsigned char sym_rec[5];
            sym_rec[0] = sym_enc[0] ^ e[0];
            sym_rec[1] = sym_enc[1] ^ e[1];
            sym_rec[2] = sym_enc[2] ^ e[2];
            sym_rec[3] = sym_enc[3] ^ e[3];
            sym_rec[4] = sym_enc[4] ^ e[4];

            // decoded symbol
            unsigned char sym_dec[4];
            int syndrome_flag = fec_secded3932_decode_symbol(sym_rec, sym_dec);


            if (liquid_autotest_verbose) {
                // print error vector
                printf("%3u, e = ", k);
                liquid_print_bitstring(e[0], 7);
                liquid_print_bitstring(e[1], 8);
                liquid_print_bitstring(e[2], 8);
                liquid_print_bitstring(e[3], 8);
                liquid_print_bitstring(e[4], 8);
                printf(" flag=%2d\n", syndrome_flag);
            }

            // validate syndrome flag is '2'
            CONTEND_EQUALITY(syndrome_flag, 2);
        }
    }
}

