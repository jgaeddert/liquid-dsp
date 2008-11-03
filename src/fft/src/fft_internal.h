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
    unsigned int n;             // fft size
    float complex * twiddle;    // twiddle factors
    float complex * x;          // input array
    float complex * y;          // output array
    int direction;              // forward/reverse
};

// execute basic dft (slow, but guarantees
// correct output)
void fft_execute_dft(fftplan _p);

// execute basic dft using look-up table for
// twiddle factors (fast for small fft sizes)
void fft_execute_twiddle_table(fftplan _p);

// execute radix-2 fft
//void fft_execute_radix2(fftplan _p);

#endif // __LIQUID_FFT_INTERNAL_H__

