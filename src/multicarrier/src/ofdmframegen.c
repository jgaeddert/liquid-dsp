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
//
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "liquid.internal.h"

#if HAVE_FFTW3_H
#   include <fftw3.h>
#endif

#define DEBUG_OFDMFRAMEGEN 1

struct ofdmframegen_s {
    unsigned int num_subcarriers;
    unsigned int cp_len;

    float complex * x; // time-domain buffer
    float complex * X; // freq-domain buffer

#if HAVE_FFTW3_H
    fftwf_plan fft;
#else
    fftplan fft;
#endif
};

ofdmframegen ofdmframegen_create(unsigned int _num_subcarriers,
                                 unsigned int _cp_len)
{
    ofdmframegen q = (ofdmframegen) malloc(sizeof(struct ofdmframegen_s));

    // allocate memory for buffers
    q->x = (float complex*) malloc((q->num_subcarriers)*sizeof(float complex));
    q->X = (float complex*) malloc((q->num_subcarriers)*sizeof(float complex));

#if HAVE_FFTW3_H
    q->fft = fftwf_plan_dft_1d(q->num_subcarriers, q->X, q->x, FFTW_BACKWARD, FFTW_ESTIMATE);
#else
    q->fft = fft_create_plan(q->num_subcarriers, q->X, q->x, FFT_REVERSE);
#endif

    return q;
}

void ofdmframegen_destroy(ofdmframegen _q)
{
    free(_q->x);
    free(_q->X);

#if HAVE_FFTW3_H
    fftwf_destroy_plan(_q->fft);
#else
    fft_destroy_plan(_q->fft);
#endif
    free(_q);
}

void ofdmframegen_print(ofdmframegen _q)
{
    printf("ofdmframegen: [%u taps]\n", 0);
}

void ofdmframegen_clear(ofdmframegen _q)
{
}

void ofdmframegen_execute(ofdmframegen _q,
                          float complex * _x,
                          float complex * _y)
{
}

