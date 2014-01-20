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
// script to generate c_ones_mod2 table
//

#include <stdio.h>

// count the number of ones in a byte
unsigned int count_ones(unsigned char);

int main() {

    unsigned char c_ones_mod2[256];

    unsigned int b;
    for (b=0; b<256; b++)
        c_ones_mod2[b] = count_ones((unsigned char)b) % 2;
    
    // print results
    printf("unsigned char c_ones_mod2[] = {\n    ");
    for (b=0; b<256; b++) {
        printf("%1u", c_ones_mod2[b]);
        if (b==255)
            printf("\n");
        else if (((b+1)%16)==0)
            printf(",\n    ");
        else
            printf(", ");
    }
    printf("};\n");
    return 0;
}

// count the number of ones in a byte
unsigned int count_ones(unsigned char _b)
{
    unsigned int i, n=0;
    for (i=0; i<8; i++) {
        n += _b & 0x01;
        _b >>= 1;
    }
    return n;
}
