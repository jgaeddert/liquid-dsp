#ifndef __FFT_AUTOTEST_H__
#define __FFT_AUTOTEST_H__

#include "../../../autotest/autotest.h"
#include "../src/fft.h"

float complex x16[] = {
  -0.686691 - 0.335976*_Complex_I,
  -0.864415 - 0.001461*_Complex_I,
  -0.647861 + 0.205138*_Complex_I,
  -0.712670 + 0.101043*_Complex_I,
   1.394510 + 1.936140*_Complex_I,
   0.771956 + 0.303301*_Complex_I,
   0.238924 + 0.012978*_Complex_I,
   0.610194 + 0.395016*_Complex_I,
  -0.409003 - 1.551359*_Complex_I,
   1.152638 + 1.611616*_Complex_I,
  -0.055798 + 0.802441*_Complex_I,
   0.413924 - 0.796679*_Complex_I,
   0.861930 - 1.920219*_Complex_I,
  -1.651307 + 0.948410*_Complex_I,
   1.364860 - 2.205625*_Complex_I,
   0.058373 + 0.691752*_Complex_I
};

float complex test16[] = {
   1.83956 + 0.19651*_Complex_I,
   0.87335 - 1.35653*_Complex_I,
   0.35080 + 1.77638*_Complex_I,
  -2.92965 + 4.87470*_Complex_I,
   2.73137 + 0.27458*_Complex_I,
  -2.10296 + 5.23819*_Complex_I,
  -9.06885 - 5.86613*_Complex_I,
  -1.66865 - 1.46952*_Complex_I,
   2.28218 - 6.30949*_Complex_I,
   9.33189 + 1.16945*_Complex_I,
  -0.65460 - 0.96802*_Complex_I,
  -3.80060 + 5.03314*_Complex_I,
  -2.21011 - 1.64728*_Complex_I,
   6.21240 - 2.31991*_Complex_I,
  -4.03587 - 2.55527*_Complex_I,
  -8.13731 - 1.44647*_Complex_I
};

// autotest helper function
void fft_test(float complex *_x, float complex *_test, unsigned int _n)
{
    float tol=1e-4f;

    unsigned int i;

    float complex y[_n], z[_n];

    // compute FFT
    fftplan pf = fft_create_plan(_n, _x, y, FFT_FORWARD);
    fft_execute(pf);

    // compute IFFT
    fftplan pr = fft_create_plan(_n, y, z, FFT_REVERSE);
    fft_execute(pr);

    // normalize inverse
    for (i=0; i<_n; i++)
        z[i] /= (float) _n;

    // validate results
    float fft_error, ifft_error;
    for (i=0; i<_n; i++) {
        fft_error = cabsf( y[i] - _test[i] );
        ifft_error = cabsf( _x[i] - z[i] );
        CONTEND_DELTA( fft_error, 0, tol);
        CONTEND_DELTA( ifft_error, 0, tol);
    }

    // destroy plans
    fft_destroy_plan(pf);
    fft_destroy_plan(pr);
}

// 
// AUTOTESTS: n-point ffts
//
//void xautotest_fft_4()       { fft_test(x4,      test4,      4);     }
void autotest_fft_16()      { fft_test(x16,     test16,     16);    }

#endif 

