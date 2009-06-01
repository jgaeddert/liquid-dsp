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

#ifndef __CHECKSUM_AUTOTEST_H__
#define __CHECKSUM_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

//
// AUTOTEST: basic checksum
//
void autotest_basic_checksum()
{
    unsigned char data[] = {0x25, 0x62, 0x3F, 0x52};
    unsigned char key = checksum_generate_key(data, 4);

    // contend data/key are valid
    CONTEND_EXPRESSION(checksum_validate_message(data, 4, key));

    // corrupt data
    data[0]++;

    // contend data/key are invalid
    CONTEND_EXPRESSION(!checksum_validate_message(data, 4, key));
}

#endif 

