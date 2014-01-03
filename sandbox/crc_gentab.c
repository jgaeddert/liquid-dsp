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
// CRC table generator
//

#include <stdio.h>
#include <stdlib.h>

// generator polynomial
#define CRC32_POLY 0xEDB88320 // 0x04C11DB7

int main() {
    unsigned int crc;
    unsigned int mask;
    unsigned int crcpoly = CRC32_POLY;

    unsigned int crc_gentab[256];

    // generate table
    unsigned int i, j;
    for (i=0; i<256; i++) {
        crc = i;
        for (j=0; j<8; j++) {
            mask = -(crc & 0x01);
            crc = (crc >> 1) ^ (crcpoly & mask);
        }
        crc_gentab[i] = crc;
    }

    printf("\n");
    printf("unsigned char crc_gentab[%u] = {\n    ", 256);
    for (i=0; i<256; i++) {
        printf("0x%.2x", crc_gentab[i]);
        if (i==255)
            printf("};\n");
        else if (((i+1)%8)==0)
            printf(",\n    ");
        else
            printf(", ");
    }

    // test it...

    return 0;
}

