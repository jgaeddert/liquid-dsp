/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <string.h>

#include "autotest/autotest.h"
#include "liquid.h"

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
    CONTEND_EQUALITY(liquid_reverse_byte(b),r);
}

//
// AUTOTEST: reverse uint16_t
//
void autotest_reverse_uint16()
{
    // 1111 0111 0101 1001
    unsigned int b = 0xF759;

    // 1001 1010 1110 1111
    unsigned int r = 0x9AEF;

    // 
    CONTEND_EQUALITY(liquid_reverse_uint16(b),r);
}



//
// AUTOTEST: reverse uint32_t
//
void autotest_reverse_uint32()
{
    // 0110 0010 1101 1001 0011 1011 1111 0000
    unsigned int b = 0x62D93BF0;

    // 0000 1111 1101 1100 1001 1011 0100 0110
    unsigned int r = 0x0FDC9B46;

    // 
    CONTEND_EQUALITY(liquid_reverse_uint32(b),r);
}

// 
// autotest helper function
//
void validate_crc(crc_scheme _check,
                  unsigned int _n)
{
    unsigned int i;

    // generate pseudo-random data
    unsigned char data[_n];
    msequence ms = msequence_create_default(9);
    for (i=0; i<_n; i++)
        data[i] = msequence_generate_symbol(ms,8);
    msequence_destroy(ms);

    // generate key
    unsigned int key = crc_generate_key(_check, data, _n);

    // contend data/key are valid
    CONTEND_EXPRESSION(crc_validate_message(_check, data, _n, key));

    //
    unsigned char data_corrupt[_n];
    unsigned int j;
    for (i=0; i<_n; i++) {
        for (j=0; j<8; j++) {
            // copy original data sequence
            memmove(data_corrupt, data, _n*sizeof(unsigned char));

            // flip bit j at byte i
            data[i] ^= (1 << j);

            // contend data/key are invalid
            CONTEND_EXPRESSION(crc_validate_message(_check, data, _n, key)==0);
        }
    }
}

// 
// AUTOTESTS : validate error-detection tests
//
void autotest_checksum() { validate_crc(LIQUID_CRC_CHECKSUM,    16); }
void autotest_crc8()     { validate_crc(LIQUID_CRC_8,           16); }
void autotest_crc16()    { validate_crc(LIQUID_CRC_16,          64); }
void autotest_crc24()    { validate_crc(LIQUID_CRC_24,          64); }
void autotest_crc32()    { validate_crc(LIQUID_CRC_32,          64); }


