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

#ifndef __FFT_AUTOTEST_H__
#define __FFT_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

#include "fft_helper.h"
#include "fft_data.h"

// 
// AUTOTESTS: n-point ffts
//
void autotest_fft_4()       { fft_test(x4,      test4,      4);     }
void autotest_fft_5()       { fft_test(x5,      test5,      5);     }
void autotest_fft_6()       { fft_test(x6,      test6,      6);     }
void autotest_fft_7()       { fft_test(x7,      test7,      7);     }
void autotest_fft_8()       { fft_test(x8,      test8,      8);     }
void autotest_fft_9()       { fft_test(x9,      test9,      9);     }
void autotest_fft_10()      { fft_test(x10,     test10,     10);    }
void autotest_fft_16()      { fft_test(x16,     test16,     16);    }
void autotest_fft_20()      { fft_test(x20,     test20,     20);    }

#endif 

