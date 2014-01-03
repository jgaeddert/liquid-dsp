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
// generate table for byte reversal
//

#include <stdio.h>
#include <stdlib.h>

// slow implementation of byte reversal
unsigned char reverse_byte(unsigned char _x)
{
    unsigned char y = 0x00;
    unsigned int i;
    for (i=0; i<8; i++) {
        y <<= 1;
        y |= _x & 1;
        _x >>= 1;
    }
    return y;
}


int main()
{
    printf("// auto-generated file (do not edit)\n");
    printf("\n");
    printf("// reverse byte table\n");
    printf("unsigned const char liquid_reverse_byte[256] = {\n    ");
    unsigned int i;
    for (i=0; i<256; i++) {
        // reverse byte
        unsigned char byte_rev = reverse_byte((unsigned char)i);

        // print results
        printf("0x%.2x", byte_rev);

        if ( i != 255 ) {
            printf(",");
            if ( ((i+1)%8)==0 )
                printf("\n    ");
            else
                printf(" ");
        }

    }
    printf("};\n\n");
    
    return 0;
}
