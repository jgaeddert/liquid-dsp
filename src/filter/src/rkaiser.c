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

//
// Design root-Nyquist Kiser filter
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "liquid.internal.h"

#if HAVE_FFTW3_H
#   include <fftw3.h>
#endif

void design_rkaiser_filter(
  unsigned int _k,
  unsigned int _m,
  float _beta,
  float _dt,
  float * _h
)
{
    unsigned int h_len;

    if ( _k < 1 ) {
        printf("error: design_rkaiser_filter(): k must be greater than 0\n");
        exit(0);
    } else if ( _m < 1 ) {
        printf("error: design_rkaiser_filter(): m must be greater than 0\n");
        exit(0);
    } else if ( (_beta < 0.0f) || (_beta > 1.0f) ) {
        printf("error: design_rkaiser_filter(): beta must be in [0,1]\n");
        exit(0);
    } else;

    unsigned int i;

    h_len = 2*_k*_m + 1;

    // 
    float fc = 1.0f / (float)(_k);  // filter cut-off

    // scaling factor due to transforms, filter gain
    float zeta = sqrtf((float)_k) / ((float)h_len);

    // memory arrays
    float hf[h_len];
    float complex h[h_len];
    float complex H[h_len];
    float complex G[h_len];
    float complex g[h_len];

    // transform objects
#if HAVE_FFTW3_H
    fftwf_plan  fft = fftwf_plan_dft_1d(h_len,h,H,FFTW_FORWARD,FFTW_ESTIMATE);
    fftwf_plan ifft = fftwf_plan_dft_1d(h_len,G,g,FFTW_BACKWARD,FFTW_ESTIMATE);
#else
    fftplan  fft = fft_create_plan(h_len,h,H,FFT_FORWARD);
    fftplan ifft = fft_create_plan(h_len,G,g,FFT_REVERSE);
#endif

    // design filter from Kaiser prototype (nyquist filter)
    fir_kaiser_window(h_len, fc, 60.0f, 0.0f, hf);

    // copy to complex array, shifting values appropriately
    for (i=0; i<h_len; i++)
        h[(i+_k*_m+1)%h_len] = hf[i];

    // run forward transform
#if HAVE_FFTW3_H
    fftwf_execute(fft);
#else
    fft_execute(fft);
#endif

    // compute spectral factor
    for (i=0; i<h_len; i++)
        G[i] = sqrtf(fabsf(crealf(H[i])));

    // apply phase rotation (time delay)
    // ...

    // compute reverse transform
#if HAVE_FFTW3_H
    fftwf_execute(ifft);
#else
    fft_execute(ifft);
#endif

    // copy to real array, shifting values appropriately
    for (i=0; i<h_len; i++)
        _h[i] = crealf(g[(i+_k*_m+1)%h_len]) * zeta;

    // destroy transform objects
#if HAVE_FFTW3_H
    fftwf_destroy_plan(fft);
    fftwf_destroy_plan(ifft);
#else
    fft_destroy_plan(fft);
    fft_destroy_plan(ifft);
#endif

#if 1
    // print filter coefficients
    printf("-----\n");
    for (i=0; i<h_len; i++)
        printf("hf(%3u) = %12.8f;\n", i+1, hf[i]);
    printf("-----\n");
    for (i=0; i<h_len; i++)
        printf("h(%3u) = %12.8f;\n", i+1, crealf(h[i]));
    printf("-----\n");
    for (i=0; i<h_len; i++)
        printf("H(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(H[i]), cimagf(H[i]));
    printf("-----\n");
    for (i=0; i<h_len; i++)
        printf("G(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(G[i]), cimagf(G[i]));
    printf("-----\n");
    for (i=0; i<h_len; i++)
        printf("g(%3u) = %12.8f + j*%12.8f;\n", i+1, crealf(g[i]), cimagf(g[i]));
    printf("-----\n");
    for (i=0; i<h_len; i++)
        printf("h(%3u) = %12.8f;\n", i+1, _h[i]);
#endif
}

