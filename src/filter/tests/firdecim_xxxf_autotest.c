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
// firdecim_xxxf_autotest.c : test floating-point filters
//

#include "autotest/autotest.h"
#include "liquid.h"

// autotest data definitions
#include "src/filter/tests/firdecim_autotest.h"

// 
// AUTOTEST: firdecim_rrrf tests
//
void autotest_firdecim_rrrf_data_M2h4x20()
{
    firdecim_rrrf_test(2,
                       firdecim_rrrf_data_M2h4x20_h, 4,
                       firdecim_rrrf_data_M2h4x20_x, 20,
                       firdecim_rrrf_data_M2h4x20_y, 10);
}
void autotest_firdecim_rrrf_data_M3h7x30()
{
    firdecim_rrrf_test(3,
                       firdecim_rrrf_data_M3h7x30_h, 7,
                       firdecim_rrrf_data_M3h7x30_x, 30,
                       firdecim_rrrf_data_M3h7x30_y, 10);
}
void autotest_firdecim_rrrf_data_M4h13x40()
{
    firdecim_rrrf_test(4,
                       firdecim_rrrf_data_M4h13x40_h, 13,
                       firdecim_rrrf_data_M4h13x40_x, 40,
                       firdecim_rrrf_data_M4h13x40_y, 10);
}
void autotest_firdecim_rrrf_data_M5h23x50()
{
    firdecim_rrrf_test(5,
                       firdecim_rrrf_data_M5h23x50_h, 23,
                       firdecim_rrrf_data_M5h23x50_x, 50,
                       firdecim_rrrf_data_M5h23x50_y, 10);
}


// 
// AUTOTEST: firdecim_crcf tests
//
void autotest_firdecim_crcf_data_M2h4x20()
{
    firdecim_crcf_test(2,
                       firdecim_crcf_data_M2h4x20_h, 4,
                       firdecim_crcf_data_M2h4x20_x, 20,
                       firdecim_crcf_data_M2h4x20_y, 8);
}
void autotest_firdecim_crcf_data_M3h7x30()
{
    firdecim_crcf_test(3,
                       firdecim_crcf_data_M3h7x30_h, 7,
                       firdecim_crcf_data_M3h7x30_x, 30,
                       firdecim_crcf_data_M3h7x30_y, 10);
}
void autotest_firdecim_crcf_data_M4h13x40()
{
    firdecim_crcf_test(4,
                       firdecim_crcf_data_M4h13x40_h, 13,
                       firdecim_crcf_data_M4h13x40_x, 40,
                       firdecim_crcf_data_M4h13x40_y, 10);
}
void autotest_firdecim_crcf_data_M5h23x50()
{
    firdecim_crcf_test(5,
                       firdecim_crcf_data_M5h23x50_h, 23,
                       firdecim_crcf_data_M5h23x50_x, 50,
                       firdecim_crcf_data_M5h23x50_y, 10);
}


// 
// AUTOTEST: firdecim_cccf tests
//
void autotest_firdecim_cccf_data_M2h4x20()
{
    firdecim_cccf_test(2,
                       firdecim_cccf_data_M2h4x20_h, 4,
                       firdecim_cccf_data_M2h4x20_x, 20,
                       firdecim_cccf_data_M2h4x20_y, 8);
}
void autotest_firdecim_cccf_data_M3h7x30()
{
    firdecim_cccf_test(3,
                       firdecim_cccf_data_M3h7x30_h, 7,
                       firdecim_cccf_data_M3h7x30_x, 30,
                       firdecim_cccf_data_M3h7x30_y, 10);
}
void autotest_firdecim_cccf_data_M4h13x40()
{
    firdecim_cccf_test(4,
                       firdecim_cccf_data_M4h13x40_h, 13,
                       firdecim_cccf_data_M4h13x40_x, 40,
                       firdecim_cccf_data_M4h13x40_y, 10);
}
void autotest_firdecim_cccf_data_M5h23x50()
{
    firdecim_cccf_test(5,
                       firdecim_cccf_data_M5h23x50_h, 23,
                       firdecim_cccf_data_M5h23x50_x, 50,
                       firdecim_cccf_data_M5h23x50_y, 10);
}


