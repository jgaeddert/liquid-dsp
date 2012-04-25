/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2012 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2012 Virginia Polytechnic
 *                                      Institute & State University
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

#include "autotest/autotest.h"
#include "liquid.h"

// autotest data definitions
#include "src/fft/tests/fft_autotest.h"

// 
// AUTOTESTS: fixed-point FFTs
//
void autotest_fftq16_4()       { fftq16_test( fft_test_x4,   fft_test_y4,      4);     }
void autotest_fftq16_5()       { fftq16_test( fft_test_x5,   fft_test_y5,      5);     }
void autotest_fftq16_6()       { fftq16_test( fft_test_x6,   fft_test_y6,      6);     }
void autotest_fftq16_7()       { fftq16_test( fft_test_x7,   fft_test_y7,      7);     }
void autotest_fftq16_8()       { fftq16_test( fft_test_x8,   fft_test_y8,      8);     }
void autotest_fftq16_9()       { fftq16_test( fft_test_x9,   fft_test_y9,      9);     }
void autotest_fftq16_16()      { fftq16_test( fft_test_x16,  fft_test_y16,     16);    }
void autotest_fftq16_20()      { fftq16_test( fft_test_x20,  fft_test_y20,     20);    }
void autotest_fftq16_32()      { fftq16_test( fft_test_x32,  fft_test_y32,     32);    }
void autotest_fftq16_64()      { fftq16_test( fft_test_x64,  fft_test_y64,     64);    }

