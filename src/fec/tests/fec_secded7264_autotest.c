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

#include <stdlib.h>
#include <stdio.h>

#include "autotest/autotest.h"
#include "liquid.internal.h"

//
// AUTOTEST: SEC-DEC (72,64) codec (single error)
//
void autotest_secded7264_codec()
{
    unsigned int k; // error location

    for (k=0; k<72; k++) {
        // generate symbol
        unsigned int sym_org[2];
        sym_org[0] = rand() & 0xffffffff;
        sym_org[1] = rand() & 0xffffffff;

        // encoded symbol
        unsigned int sym_enc[3];
        fec_secded7264_encode_symbol(sym_org, sym_enc);

        // generate error vector (single error)
        unsigned int e[3] = {0,0,0};
        div_t d = div(k+24,32);
        e[d.quot] = 1 << (32-d.rem-1);

        // received symbol
        unsigned int sym_rec[3];
        sym_rec[0] = sym_enc[0] ^ e[0];
        sym_rec[1] = sym_enc[1] ^ e[1];
        sym_rec[2] = sym_enc[2] ^ e[2];

        // decoded symbol
        unsigned int sym_dec[2];
        fec_secded7264_decode_symbol(sym_rec, sym_dec);

        // validate data are the same
        CONTEND_EQUALITY(sym_org[0], sym_dec[0]);
        CONTEND_EQUALITY(sym_org[1], sym_dec[1]);
    }
}

