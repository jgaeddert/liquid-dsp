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

#define DEBUG_OFDMFRAMEGEN                  1
#define OFDMFRAMEGEN_MIN_NUM_SUBCARRIERS    (8)

struct ofdmframegen_s {
    unsigned int num_subcarriers;
    unsigned int cp_len;

    float complex * x;      // time-domain buffer
    float complex * X;      // freq-domain buffer

    float complex * xcp;    // cyclic prefix pointer (not allocated)

#if HAVE_FFTW3_H
    fftwf_plan fft;
#else
    fftplan fft;
#endif
    float zeta;             // fft scaling factor
};

ofdmframegen ofdmframegen_create(unsigned int _num_subcarriers,
                                 unsigned int _cp_len)
{
    ofdmframegen q = (ofdmframegen) malloc(sizeof(struct ofdmframegen_s));

    // error-checking
    if (_num_subcarriers < OFDMFRAMEGEN_MIN_NUM_SUBCARRIERS) {
        printf("error: ofdmframegen_create(), num_subcarriers (%u) below minimum (%u)\n",
                _num_subcarriers, OFDMFRAMEGEN_MIN_NUM_SUBCARRIERS);
        exit(1);
    } else if (_cp_len < 1) {
        printf("error: ofdmframegen_create(), cp_len must be greater than 0\n");
        exit(1);
    } else if (_cp_len > _num_subcarriers) {
        printf("error: ofdmframegen_create(), cp_len (%u) must be less than number of subcarriers(%u)\n",
                _cp_len, _num_subcarriers);
        exit(1);
    }

    q->num_subcarriers = _num_subcarriers;
    q->cp_len = _cp_len;

    // allocate memory for buffers
    q->x = (float complex*) malloc((q->num_subcarriers)*sizeof(float complex));
    q->X = (float complex*) malloc((q->num_subcarriers)*sizeof(float complex));

#if HAVE_FFTW3_H
    q->fft = fftwf_plan_dft_1d(q->num_subcarriers, q->X, q->x, FFTW_BACKWARD, FFTW_ESTIMATE);
#else
    q->fft = fft_create_plan(q->num_subcarriers, q->X, q->x, FFT_REVERSE);
#endif
    q->zeta = 1.0f / sqrtf((float)(q->num_subcarriers));

    // set cyclic prefix array pointer
    q->xcp = &(q->x[q->num_subcarriers - q->cp_len]);

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
    printf("ofdmframegen:\n");
    printf("    num subcarriers     :   %u\n", _q->num_subcarriers);
    printf("    cyclic prefix len   :   %u (%6.2f%%)\n",
                    _q->cp_len,
                    100.0f*(float)(_q->cp_len)/(float)(_q->num_subcarriers));
}

void ofdmframegen_clear(ofdmframegen _q)
{
}

void ofdmframegen_execute(ofdmframegen _q,
                          float complex * _x,
                          float complex * _y)
{
    // move frequency data to internal buffer
    memmove(_q->X, _x, (_q->num_subcarriers)*sizeof(float complex));

    // execute inverse fft, store in buffer _q->x
#if HAVE_FFTW3_H
    fftwf_execute(_q->fft);
#else
    fft_execute(_q->fft);
#endif

    // scaling
    unsigned int i;
    for (i=0; i<_q->num_subcarriers; i++)
        _q->x[i] *= _q->zeta;

    // copy cyclic prefix
    memmove(_y, _q->xcp, (_q->cp_len)*sizeof(float complex));

    // copy remainder of signal
    memmove(&_y[_q->cp_len], _q->x, (_q->num_subcarriers)*sizeof(float complex));
}

