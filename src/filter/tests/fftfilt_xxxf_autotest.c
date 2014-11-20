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
// fftfilt_xxxf_autotest.c : test floating-point filters
//

#include "autotest/autotest.h"
#include "liquid.h"

// autotest data definitions
#include "src/filter/tests/fftfilt_autotest.h"

// 
// AUTOTEST: fftfilt_rrrf tests
//
void autotest_fftfilt_rrrf_data_h4x256()
{
    fftfilt_rrrf_test(fftfilt_rrrf_data_h4x256_h, 4,
                      fftfilt_rrrf_data_h4x256_x, 256,
                      fftfilt_rrrf_data_h4x256_y, 256);
}
void autotest_fftfilt_rrrf_data_h7x256()
{
    fftfilt_rrrf_test(fftfilt_rrrf_data_h7x256_h, 7,
                      fftfilt_rrrf_data_h7x256_x, 256,
                      fftfilt_rrrf_data_h7x256_y, 256);
}
void autotest_fftfilt_rrrf_data_h13x256()
{
    fftfilt_rrrf_test(fftfilt_rrrf_data_h13x256_h, 13,
                      fftfilt_rrrf_data_h13x256_x, 256,
                      fftfilt_rrrf_data_h13x256_y, 256);
}
void autotest_fftfilt_rrrf_data_h23x256()
{
    fftfilt_rrrf_test(fftfilt_rrrf_data_h23x256_h, 23,
                      fftfilt_rrrf_data_h23x256_x, 256,
                      fftfilt_rrrf_data_h23x256_y, 256);
}


// 
// AUTOTEST: fftfilt_crcf tests
//
void autotest_fftfilt_crcf_data_h4x256()
{
    fftfilt_crcf_test(fftfilt_crcf_data_h4x256_h, 4,
                      fftfilt_crcf_data_h4x256_x, 256,
                      fftfilt_crcf_data_h4x256_y, 256);
}
void autotest_fftfilt_crcf_data_h7x256()
{
    fftfilt_crcf_test(fftfilt_crcf_data_h7x256_h, 7,
                      fftfilt_crcf_data_h7x256_x, 256,
                      fftfilt_crcf_data_h7x256_y, 256);
}
void autotest_fftfilt_crcf_data_h13x256()
{
    fftfilt_crcf_test(fftfilt_crcf_data_h13x256_h, 13,
                      fftfilt_crcf_data_h13x256_x, 256,
                      fftfilt_crcf_data_h13x256_y, 256);
}
void autotest_fftfilt_crcf_data_h23x256()
{
    fftfilt_crcf_test(fftfilt_crcf_data_h23x256_h, 23,
                      fftfilt_crcf_data_h23x256_x, 256,
                      fftfilt_crcf_data_h23x256_y, 256);
}


// 
// AUTOTEST: fftfilt_cccf tests
//
void autotest_fftfilt_cccf_data_h4x256()
{
    fftfilt_cccf_test(fftfilt_cccf_data_h4x256_h, 4,
                      fftfilt_cccf_data_h4x256_x, 256,
                      fftfilt_cccf_data_h4x256_y, 256);
}
void autotest_fftfilt_cccf_data_h7x256()
{
    fftfilt_cccf_test(fftfilt_cccf_data_h7x256_h, 7,
                      fftfilt_cccf_data_h7x256_x, 256,
                      fftfilt_cccf_data_h7x256_y, 256);
}
void autotest_fftfilt_cccf_data_h13x256()
{
    fftfilt_cccf_test(fftfilt_cccf_data_h13x256_h, 13,
                      fftfilt_cccf_data_h13x256_x, 256,
                      fftfilt_cccf_data_h13x256_y, 256);
}
void autotest_fftfilt_cccf_data_h23x256()
{
    fftfilt_cccf_test(fftfilt_cccf_data_h23x256_h, 23,
                      fftfilt_cccf_data_h23x256_x, 256,
                      fftfilt_cccf_data_h23x256_y, 256);
}


