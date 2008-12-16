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

//
// ascii spectrogram
//
typedef struct asgram_s * asgram;
asgram asgram_create(float complex *_x, unsigned int _n);
void asgram_set_scale(asgram _q, float _scale);
void asgram_set_offset(asgram _q, float _offset);
void asgram_destroy(asgram _q);
void asgram_execute(asgram _q);

#endif // __LIQUID_FFT_H__

