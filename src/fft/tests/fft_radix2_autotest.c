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
// fft_radix2_autotest.c : test power-of-two transforms
//

#include "autotest/autotest.h"
#include "liquid.h"

// autotest data definitions
#include "src/fft/tests/fft_runtest.h"

// 
// AUTOTESTS: power-of-two transforms
//
void autotest_fft_2()       { fft_test( fft_test_x2,   fft_test_y2,      2);     }
void autotest_fft_4()       { fft_test( fft_test_x4,   fft_test_y4,      4);     }
void autotest_fft_8()       { fft_test( fft_test_x8,   fft_test_y8,      8);     }
void autotest_fft_16()      { fft_test( fft_test_x16,  fft_test_y16,     16);    }
void autotest_fft_32()      { fft_test( fft_test_x32,  fft_test_y32,     32);    }
void autotest_fft_64()      { fft_test( fft_test_x64,  fft_test_y64,     64);    }

