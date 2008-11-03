#ifndef __FFT_AUTOTEST_H__
#define __FFT_AUTOTEST_H__

#include "../../../autotest/autotest.h"
#include "../src/fft.h"

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

