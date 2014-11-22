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
// fft_prime_autotest.c : test FFTs of prime length
//

#include "autotest/autotest.h"
#include "liquid.h"

// autotest data definitions
#include "src/fft/tests/fft_runtest.h"

// 
// AUTOTESTS: n-point ffts
//
void autotest_fft_17()  { fft_test( fft_test_x17,   fft_test_y17,   17); }
void autotest_fft_43()  { fft_test( fft_test_x43,   fft_test_y43,   43); }
void autotest_fft_79()  { fft_test( fft_test_x79,   fft_test_y79,   79); }
void autotest_fft_157() { fft_test( fft_test_x157,  fft_test_y157, 157); }
void autotest_fft_317() { fft_test( fft_test_x317,  fft_test_y317, 317); }
void autotest_fft_509() { fft_test( fft_test_x509,  fft_test_y509, 509); }
