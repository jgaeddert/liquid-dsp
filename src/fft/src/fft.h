//
// fft : inefficient but effective
//

#ifndef __LIQUID_FFT_H__
#define __LIQUID_FFT_H__

#include <complex.h>

typedef struct fftplan_s * fftplan;

#define FFT_FORWARD 0
#define FFT_REVERSE 1
fftplan fft_create_plan(unsigned int _n, float complex * _x, float complex * _y, int _dir);
void fft_destroy_plan(fftplan _p);
void fft_execute(fftplan _p);

void fft_shift(float complex *_x, unsigned int _n);

#endif // __LIQUID_FFT_H__

