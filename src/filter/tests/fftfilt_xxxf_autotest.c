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


