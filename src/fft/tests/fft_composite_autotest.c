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
// fft_composite_autotest.c : test FFTs of 'composite' length (not
//   prime, not of form 2^m)
//

#include "autotest/autotest.h"
#include "liquid.h"

// autotest data definitions
#include "src/fft/tests/fft_runtest.h"

// 
// AUTOTESTS: n-point ffts
//
void autotest_fft_10()  { fft_test( fft_test_x10,   fft_test_y10,   10); }
void autotest_fft_21()  { fft_test( fft_test_x21,   fft_test_y21,   21); }
void autotest_fft_22()  { fft_test( fft_test_x22,   fft_test_y22,   22); }
void autotest_fft_24()  { fft_test( fft_test_x24,   fft_test_y24,   24); }
void autotest_fft_26()  { fft_test( fft_test_x26,   fft_test_y26,   26); }
void autotest_fft_30()  { fft_test( fft_test_x30,   fft_test_y30,   30); }
void autotest_fft_35()  { fft_test( fft_test_x35,   fft_test_y35,   35); }
void autotest_fft_36()  { fft_test( fft_test_x36,   fft_test_y36,   36); }
void autotest_fft_48()  { fft_test( fft_test_x48,   fft_test_y48,   48); }
void autotest_fft_63()  { fft_test( fft_test_x63,   fft_test_y63,   63); }
void autotest_fft_92()  { fft_test( fft_test_x92,   fft_test_y92,   92); }
void autotest_fft_96()  { fft_test( fft_test_x96,   fft_test_y96,   96); }

void autotest_fft_120() { fft_test( fft_test_x120,  fft_test_y120, 120); }
void autotest_fft_130() { fft_test( fft_test_x130,  fft_test_y130, 130); }
void autotest_fft_192() { fft_test( fft_test_x192,  fft_test_y192, 192); }

