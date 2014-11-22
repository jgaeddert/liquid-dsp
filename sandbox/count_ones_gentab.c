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
// generate tables for counting ones in a byte
//

#include <stdio.h>
#include <stdlib.h>

// slow implementation counting ones in a byte
unsigned int count_ones(unsigned char _x)
{
    unsigned int n=0;
    unsigned int i;
    for (i=0; i<8; i++)
        n += (_x >> i) & 0x01;
    
    return n;
}

int main()
{
    unsigned int i;

    printf("// auto-generated file (do not edit)\n");
    printf("\n");
    printf("// number of ones in a byte\n");
    printf("unsigned const char liquid_c_ones[256] = {\n    ");
    for (i=0; i<256; i++) {
        // 
        unsigned int n = count_ones((unsigned char)i);

        // print results
        printf("%1u", n);

        if ( i != 255 ) {
            printf(",");
            if ( ((i+1)%16)==0 )
                printf("\n    ");
            else
                printf(" ");
        }

    }
    printf("};\n\n");

    // do the same modulo 2
    printf("// number of ones in a byte modulo 2\n");
    printf("unsigned const char liquid_c_ones_mod2[256] = {\n    ");
    for (i=0; i<256; i++) {
        // 
        unsigned int n = count_ones((unsigned char)i);

        // print results
        printf("%1u", n % 2);

        if ( i != 255 ) {
            printf(",");
            if ( ((i+1)%16)==0 )
                printf("\n    ");
            else
                printf(" ");
        }

    }
    printf("};\n\n");

    
    return 0;
}
