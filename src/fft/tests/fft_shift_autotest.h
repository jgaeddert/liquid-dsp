#ifndef __FFT_SHIFT_AUTOTEST_H__
#define __FFT_SHIFT_AUTOTEST_H__

#include "../../../autotest/autotest.h"
#include "../src/fft.h"

void autotest_fft_shift_4()
{
    float complex x[] = {
        0 + 0*_Complex_I,
        1 + 1*_Complex_I,
        2 + 2*_Complex_I,
        3 + 3*_Complex_I
    };

    float complex test[] = {
        2 + 2*_Complex_I,
        3 + 3*_Complex_I,
        0 + 0*_Complex_I,
        1 + 1*_Complex_I
    };

    fft_shift(x,4);

    CONTEND_SAME_DATA(x,test,4*sizeof(float complex));
}

#endif 

