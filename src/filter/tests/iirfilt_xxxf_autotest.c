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
// iirfilt_xxxf_autotest.c : test floating-point filters
//

#include "autotest/autotest.h"
#include "liquid.h"

// autotest data definitions
#include "src/filter/tests/iirfilt_autotest.h"

// 
// AUTOTEST: iirfilt_rrrf tests
//
void autotest_iirfilt_rrrf_h3x64()
{
    iirfilt_rrrf_test(iirfilt_rrrf_data_h3x64_b,
                      iirfilt_rrrf_data_h3x64_a, 3,
                      iirfilt_rrrf_data_h3x64_x, 64,
                      iirfilt_rrrf_data_h3x64_y, 64);
}
void autotest_iirfilt_rrrf_h5x64()
{
    iirfilt_rrrf_test(iirfilt_rrrf_data_h5x64_b,
                      iirfilt_rrrf_data_h5x64_a, 5,
                      iirfilt_rrrf_data_h5x64_x, 64,
                      iirfilt_rrrf_data_h5x64_y, 64);
}
void autotest_iirfilt_rrrf_h7x64()
{
    iirfilt_rrrf_test(iirfilt_rrrf_data_h7x64_b,
                      iirfilt_rrrf_data_h7x64_a, 7,
                      iirfilt_rrrf_data_h7x64_x, 64,
                      iirfilt_rrrf_data_h7x64_y, 64);
}


// 
// AUTOTEST: iirfilt_crcf tests
//
void autotest_iirfilt_crcf_h3x64()
{
    iirfilt_crcf_test(iirfilt_crcf_data_h3x64_b,
                      iirfilt_crcf_data_h3x64_a, 3,
                      iirfilt_crcf_data_h3x64_x, 64,
                      iirfilt_crcf_data_h3x64_y, 64);
}
void autotest_iirfilt_crcf_h5x64()
{
    iirfilt_crcf_test(iirfilt_crcf_data_h5x64_b,
                      iirfilt_crcf_data_h5x64_a, 5,
                      iirfilt_crcf_data_h5x64_x, 64,
                      iirfilt_crcf_data_h5x64_y, 64);
}
void autotest_iirfilt_crcf_h7x64()
{
    iirfilt_crcf_test(iirfilt_crcf_data_h7x64_b,
                      iirfilt_crcf_data_h7x64_a, 7,
                      iirfilt_crcf_data_h7x64_x, 64,
                      iirfilt_crcf_data_h7x64_y, 64);
}


// 
// AUTOTEST: iirfilt_cccf tests
//
void autotest_iirfilt_cccf_h3x64()
{
    iirfilt_cccf_test(iirfilt_cccf_data_h3x64_b,
                      iirfilt_cccf_data_h3x64_a, 3,
                      iirfilt_cccf_data_h3x64_x, 64,
                      iirfilt_cccf_data_h3x64_y, 64);
}
void autotest_iirfilt_cccf_h5x64()
{
    iirfilt_cccf_test(iirfilt_cccf_data_h5x64_b,
                      iirfilt_cccf_data_h5x64_a, 5,
                      iirfilt_cccf_data_h5x64_x, 64,
                      iirfilt_cccf_data_h5x64_y, 64);
}
void autotest_iirfilt_cccf_h7x64()
{
    iirfilt_cccf_test(iirfilt_cccf_data_h7x64_b,
                      iirfilt_cccf_data_h7x64_a, 7,
                      iirfilt_cccf_data_h7x64_x, 64,
                      iirfilt_cccf_data_h7x64_y, 64);
}


