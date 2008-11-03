//
// fft : inefficient but effective
//

#ifndef __LIQUID_FFT_INTERNAL_H__
#define __LIQUID_FFT_INTERNAL_H__

#include <complex.h>
#include "fft.h"

// fft size below which twiddle factors
// are stored in look-up table (very fast)
#define FFT_SIZE_TWIDDLE    16

struct fftplan_s {
    unsigned int n;
    unsigned int len;
    float complex * twiddle;
    float complex * x;
    float complex * y;
    int direction;
};

void fft_execute_dft(fftplan _p);
void fft_execute_twiddle_table(fftplan _p);

#endif // __LIQUID_FFT_INTERNAL_H__

