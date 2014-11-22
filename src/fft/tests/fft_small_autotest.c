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
// fft_small_autotest.c : test small transforms
//

#include "autotest/autotest.h"
#include "liquid.h"

// autotest data definitions
#include "src/fft/tests/fft_runtest.h"

// 
// AUTOTESTS: small FFTs
//
void autotest_fft_3()       { fft_test( fft_test_x4,   fft_test_y4,      4);     }
void autotest_fft_5()       { fft_test( fft_test_x5,   fft_test_y5,      5);     }
void autotest_fft_6()       { fft_test( fft_test_x6,   fft_test_y6,      6);     }
void autotest_fft_7()       { fft_test( fft_test_x7,   fft_test_y7,      7);     }
void autotest_fft_9()       { fft_test( fft_test_x9,   fft_test_y9,      9);     }

