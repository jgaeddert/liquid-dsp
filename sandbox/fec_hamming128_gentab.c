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

//
// 2/3-rate (12,8) Hamming code encoding table generator
//

#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

int main()
{
    unsigned int i;
    unsigned int c;

    printf("unsigned short int hamming128_enc_gentab[256] = {\n    ");

    for (i=0; i<256; i++) {
        // encode symbol
        c = fec_hamming128_encode_symbol(i);

        // print result
        printf("0x%.4x", c);
        if (i != 255)
            printf(", ");
        else
            printf("};");

        if ( ((i+1)%8) == 0)
            printf("\n    ");
    }
    printf("\n");

    return 0;
}

