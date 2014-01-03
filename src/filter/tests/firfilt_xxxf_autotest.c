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
// firfilt_xxxf_autotest.c : test floating-point filters
//

#include "autotest/autotest.h"
#include "liquid.h"

// autotest data definitions
#include "src/filter/tests/firfilt_autotest.h"

// 
// AUTOTEST: firfilt_rrrf tests
//
void autotest_firfilt_rrrf_data_h4x8()
{
    firfilt_rrrf_test(firfilt_rrrf_data_h4x8_h, 4,
                      firfilt_rrrf_data_h4x8_x, 8,
                      firfilt_rrrf_data_h4x8_y, 8);
}
void autotest_firfilt_rrrf_data_h7x16()
{
    firfilt_rrrf_test(firfilt_rrrf_data_h7x16_h, 7,
                      firfilt_rrrf_data_h7x16_x, 16,
                      firfilt_rrrf_data_h7x16_y, 16);
}
void autotest_firfilt_rrrf_data_h13x32()
{
    firfilt_rrrf_test(firfilt_rrrf_data_h13x32_h, 13,
                      firfilt_rrrf_data_h13x32_x, 32,
                      firfilt_rrrf_data_h13x32_y, 32);
}
void autotest_firfilt_rrrf_data_h23x64()
{
    firfilt_rrrf_test(firfilt_rrrf_data_h23x64_h, 23,
                      firfilt_rrrf_data_h23x64_x, 64,
                      firfilt_rrrf_data_h23x64_y, 64);
}


// 
// AUTOTEST: firfilt_crcf tests
//
void autotest_firfilt_crcf_data_h4x8()
{
    firfilt_crcf_test(firfilt_crcf_data_h4x8_h, 4,
                      firfilt_crcf_data_h4x8_x, 8,
                      firfilt_crcf_data_h4x8_y, 8);
}
void autotest_firfilt_crcf_data_h7x16()
{
    firfilt_crcf_test(firfilt_crcf_data_h7x16_h, 7,
                      firfilt_crcf_data_h7x16_x, 16,
                      firfilt_crcf_data_h7x16_y, 16);
}
void autotest_firfilt_crcf_data_h13x32()
{
    firfilt_crcf_test(firfilt_crcf_data_h13x32_h, 13,
                      firfilt_crcf_data_h13x32_x, 32,
                      firfilt_crcf_data_h13x32_y, 32);
}
void autotest_firfilt_crcf_data_h23x64()
{
    firfilt_crcf_test(firfilt_crcf_data_h23x64_h, 23,
                      firfilt_crcf_data_h23x64_x, 64,
                      firfilt_crcf_data_h23x64_y, 64);
}


// 
// AUTOTEST: firfilt_cccf tests
//
void autotest_firfilt_cccf_data_h4x8()
{
    firfilt_cccf_test(firfilt_cccf_data_h4x8_h, 4,
                      firfilt_cccf_data_h4x8_x, 8,
                      firfilt_cccf_data_h4x8_y, 8);
}
void autotest_firfilt_cccf_data_h7x16()
{
    firfilt_cccf_test(firfilt_cccf_data_h7x16_h, 7,
                      firfilt_cccf_data_h7x16_x, 16,
                      firfilt_cccf_data_h7x16_y, 16);
}
void autotest_firfilt_cccf_data_h13x32()
{
    firfilt_cccf_test(firfilt_cccf_data_h13x32_h, 13,
                      firfilt_cccf_data_h13x32_x, 32,
                      firfilt_cccf_data_h13x32_y, 32);
}
void autotest_firfilt_cccf_data_h23x64()
{
    firfilt_cccf_test(firfilt_cccf_data_h23x64_h, 23,
                      firfilt_cccf_data_h23x64_x, 64,
                      firfilt_cccf_data_h23x64_y, 64);
}


