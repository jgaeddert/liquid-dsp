//
// fft : inefficient but effective
//

#ifndef __LIQUID_FFT_INTERNAL_H__
#define __LIQUID_FFT_INTERNAL_H__

#include <complex.h>
#include <stdbool.h>
#include "fft.h"

// fft size below which twiddle factors
// are stored in look-up table (very fast)
#define FFT_SIZE_LUT    16

struct fftplan_s {
    unsigned int n;             // fft size
    float complex * twiddle;    // twiddle factors
    float complex * x;          // input array
    float complex * y;          // output array
    int direction;              // forward/reverse

    // radix-2
    bool is_radix2;             // radix-2 flag
    unsigned int * index_rev;   // input indices (reversed)
    unsigned int m;             // log2(n)
};

// initialization
void fft_init_lut(fftplan _p);
void fft_init_radix2(fftplan _p);

// execution

// execute basic dft (slow, but guarantees
// correct output)
void fft_execute_dft(fftplan _p);

// execute basic dft using look-up table for
// twiddle factors (fast for small fft sizes)
void fft_execute_lut(fftplan _p);

// execute radix-2 fft
void fft_execute_radix2(fftplan _p);

// miscellaneous functions
unsigned int reverse_index(unsigned int _i, unsigned int _n);

//
// fft_shift
//
//void fft_shift_odd(float complex *_x, unsigned int _n);
//void fft_shift_even(float complex *_x, unsigned int _n);

#endif // __LIQUID_FFT_INTERNAL_H__

