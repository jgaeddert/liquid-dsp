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

#ifndef __FFT_RADIX2_AUTOTEST_H__
#define __FFT_RADIX2_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

#include "fft_helper.h"
#include "fft_radix2_data.h"

// 
// AUTOTESTS: 2^m-point ffts
//

// TODO: ensure radix-2 execution
//void xautotest_fft_r2_4()    { fft_test(xr2_4,   testr2_4,   4);     }
//void xautotest_fft_r2_8()    { fft_test(xr2_8,   testr2_8,   8);     }
//void xautotest_fft_r2_16()   { fft_test(xr2_16,  testr2_16,  16);    }
void autotest_fft_r2_32()   { fft_test(xr2_32,  testr2_32,  32);    }
void autotest_fft_r2_64()   { fft_test(xr2_64,  testr2_64,  64);    }

#endif // __FFT_RADIX2_AUTOTEST_H__

