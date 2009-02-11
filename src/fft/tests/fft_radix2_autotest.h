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

