/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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

#include <string.h>

#include "autotest/autotest.h"
#include "liquid.h"

#include "liquid.internal.h" // reverse_byte, etc.

//
// AUTOTEST: reverse byte
//
void autotest_reverse_byte()
{
    // 0110 0010
    unsigned char b = 0x62;

    // 0100 0110
    unsigned char r = 0x46;

    // 
    CONTEND_EQUALITY(reverse_byte(b),r);
}

//
// AUTOTEST: reverse uint32_t
//
void autotest_reverse_uint()
{
    // 0110 0010 1101 1001 0011 1011 1111 0000
    unsigned int b = 0x62D93BF0;

    // 0000 1111 1101 1100 1001 1011 0100 0110
    unsigned int r = 0x0FDC9B46;

    // 
    CONTEND_EQUALITY(reverse_uint32(b),r);
}


//
// AUTOTEST: crc32
//
void autotest_crc32()
{
    unsigned int i;
    unsigned int n = 64;    // input data size (number of bytes)

    // generate random data
    unsigned char data[n];
    for (i=0; i<n; i++)
        data[i] = rand() % 256;

    // generat key
    unsigned int key = crc32_generate_key(data, n);

    // contend data/key are valid
    CONTEND_EXPRESSION(crc32_validate_message(data, n, key));

    //
    unsigned char data_corrupt[n];
    unsigned int j;
    for (i=0; i<n; i++) {
        for (j=0; j<8; j++) {
            // copy original data sequence
            memmove(data_corrupt, data, n*sizeof(unsigned char));

            // flip bit j at byte i
            data[i] ^= (1 << j);

            // contend data/key are invalid
            CONTEND_EXPRESSION(!crc32_validate_message(data, n, key));
        }
    }
}



//
// AUTOTEST: crc16
//
void autotest_crc16()
{
    unsigned int i;
    unsigned int n = 64;    // input data size (number of bytes)

    // generate random data
    unsigned char data[n];
    for (i=0; i<n; i++)
        data[i] = rand() % 256;

    // generat key
    unsigned int key = crc16_generate_key(data, n);

    // contend data/key are valid
    CONTEND_EXPRESSION(crc16_validate_message(data, n, key));

    //
    unsigned char data_corrupt[n];
    unsigned int j;
    for (i=0; i<n; i++) {
        for (j=0; j<8; j++) {
            // copy original data sequence
            memmove(data_corrupt, data, n*sizeof(unsigned char));

            // flip bit j at byte i
            data[i] ^= (1 << j);

            // contend data/key are invalid
            CONTEND_EXPRESSION(!crc16_validate_message(data, n, key));
        }
    }
}


