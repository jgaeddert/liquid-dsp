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

#ifndef __CRC_AUTOTEST_H__
#define __CRC_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

#include "../src/fec_internal.h" // reverse_byte, etc.

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
    unsigned char data[] = {0x25, 0x62, 0x3F, 0x52};
    unsigned int key = crc32_generate_key(data, 4);

    // contend data/key are valid
    CONTEND_EXPRESSION(crc32_validate_message(data, 4, key));

    // corrupt data
    data[0]++;

    // contend data/key are invalid
    CONTEND_EXPRESSION(!crc32_validate_message(data, 4, key));
}

#endif 

