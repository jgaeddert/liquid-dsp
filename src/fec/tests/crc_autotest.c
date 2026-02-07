/*
 * Copyright (c) 2007 - 2026 Joseph Gaeddert
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

#include "liquid.autotest.h"
#include "liquid.internal.h"

LIQUID_AUTOTEST(reverse_byte,"test byte reversal","",0.1)
{
    // 0110 0010
    unsigned char b = 0x62;

    // 0100 0110
    unsigned char r = 0x46;

    // 
    LIQUID_CHECK(liquid_reverse_byte(b) == r);
}

LIQUID_AUTOTEST(reverse_uint16,"test reversing 16-bit word","",0.1)
{
    // 1111 0111 0101 1001
    unsigned int b = 0xF759;

    // 1001 1010 1110 1111
    unsigned int r = 0x9AEF;

    // 
    LIQUID_CHECK(liquid_reverse_uint16(b) == r);
}

LIQUID_AUTOTEST(reverse_uint32,"test reversing 32-bit word","",0.1)
{
    // 0110 0010 1101 1001 0011 1011 1111 0000
    unsigned int b = 0x62D93BF0;

    // 0000 1111 1101 1100 1001 1011 0100 0110
    unsigned int r = 0x0FDC9B46;

    // 
    LIQUID_CHECK(liquid_reverse_uint32(b) == r);
}

// autotest helper function
void testbench_crc(liquid_autotest __q__,
                   crc_scheme _check,
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
    LIQUID_CHECK(crc_validate_message(_check, data, _n, key));

    // test flipping bit value at each location in message and confirm check fails
    unsigned char data_corrupt[_n];
    unsigned int j;
    for (i=0; i<_n; i++) {
        for (j=0; j<8; j++) {
            // copy original data sequence
            memmove(data_corrupt, data, _n*sizeof(unsigned char));

            // flip bit j at byte i
            data[i] ^= (1 << j);

            // contend data/key are invalid
            LIQUID_CHECK(crc_validate_message(_check, data, _n, key)==0);
        }
    }
}

// validate error-detection tests
LIQUID_AUTOTEST(checksum,"","",0.1) { testbench_crc(__q__, LIQUID_CRC_CHECKSUM, 16); }
LIQUID_AUTOTEST(crc8,"","",0.1)     { testbench_crc(__q__, LIQUID_CRC_8,        16); }
LIQUID_AUTOTEST(crc16,"","",0.1)    { testbench_crc(__q__, LIQUID_CRC_16,       64); }
LIQUID_AUTOTEST(crc24,"","",0.1)    { testbench_crc(__q__, LIQUID_CRC_24,       64); }
LIQUID_AUTOTEST(crc32,"","",0.1)    { testbench_crc(__q__, LIQUID_CRC_32,       64); }

LIQUID_AUTOTEST(crc_config,"test CRC config","",0.1)
{
    _liquid_error_downgrade_enable();
    LIQUID_CHECK(LIQUID_OK == liquid_print_crc_schemes())

    LIQUID_CHECK(LIQUID_CRC_UNKNOWN ==     liquid_getopt_str2crc("unknown"))
    LIQUID_CHECK(LIQUID_CRC_UNKNOWN ==     liquid_getopt_str2crc("rosebud"))
    LIQUID_CHECK(LIQUID_CRC_NONE ==        liquid_getopt_str2crc("none"))
    LIQUID_CHECK(LIQUID_CRC_CHECKSUM ==    liquid_getopt_str2crc("checksum"))
    LIQUID_CHECK(LIQUID_CRC_8 ==           liquid_getopt_str2crc("crc8"))
    LIQUID_CHECK(LIQUID_CRC_16 ==          liquid_getopt_str2crc("crc16"))
    LIQUID_CHECK(LIQUID_CRC_24 ==          liquid_getopt_str2crc("crc24"))
    LIQUID_CHECK(LIQUID_CRC_32 ==          liquid_getopt_str2crc("crc32"))

    // check length
    LIQUID_CHECK(crc_get_length(LIQUID_CRC_UNKNOWN) ==   0);
    LIQUID_CHECK(crc_get_length(LIQUID_CRC_NONE) ==      0);
    LIQUID_CHECK(crc_get_length(LIQUID_CRC_CHECKSUM) ==  1);
    LIQUID_CHECK(crc_get_length(LIQUID_CRC_8) ==         1);
    LIQUID_CHECK(crc_get_length(LIQUID_CRC_16) ==        2);
    LIQUID_CHECK(crc_get_length(LIQUID_CRC_24) ==        3);
    LIQUID_CHECK(crc_get_length(LIQUID_CRC_32) ==        4);
    LIQUID_CHECK(crc_get_length(-1) ==                   0);
    _liquid_error_downgrade_disable();
}

