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

#define DEBUG_OFDMFRAMESYNC 1

struct ofdmframesync_s {
    unsigned int num_subcarriers;
    unsigned int cp_len;

    float complex * x; // time-domain buffer
    float complex * X; // freq-domain buffer

    cfwindow wcp;
    cfwindow wdelay;

#if HAVE_FFTW3_H
    fftwf_plan fft;
#else
    fftplan fft;
#endif
};

ofdmframesync ofdmframesync_create(unsigned int _num_subcarriers,
                                   unsigned int _cp_len,
                                   ofdmframesync_callback _callback,
                                   void * _userdata)
{
    ofdmframesync q = (ofdmframesync) malloc(sizeof(struct ofdmframesync_s));
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

    // cyclic prefix correlation windows
    q->wcp    = cfwindow_create(q->cp_len);
    q->wdelay = cfwindow_create(q->cp_len + q->num_subcarriers);
    return q;
}

void ofdmframesync_destroy(ofdmframesync _q)
{
    free(_q->x);
    free(_q->X);
#if HAVE_FFTW3_H
    fftwf_destroy_plan(_q->fft);
#else
    fft_destroy_plan(_q->fft);
#endif

    cfwindow_destroy(_q->wcp);
    cfwindow_destroy(_q->wdelay);
    free(_q);
}

void ofdmframesync_print(ofdmframesync _q)
{
    printf("ofdmframesync:\n");
}

void ofdmframesync_clear(ofdmframesync _q)
{
}

void ofdmframesync_execute(ofdmframesync _q,
                           float complex * _x,
                           unsigned int _n)
{
    unsigned int i;
    for (i=0; i<_n; i++)
        ofdmframesync_cpcorrelate(_q, _x[i]);
}

void ofdmframesync_cpcorrelate(ofdmframesync _q,
                               float complex _x)
{
    cfwindow_push(_q->wcp,    _x);
    cfwindow_push(_q->wdelay, _x);

    float complex * rcp;    // read pointer: cyclic prefix
    float complex * rdelay; // read pointer: delay line

    cfwindow_read(_q->wcp,    &rcp);
    cfwindow_read(_q->wdelay, &rdelay);

    // increment delay pointer
    rdelay += _q->num_subcarriers;

    float complex rxy=0.0f;
    unsigned int i;
    for (i=0; i<_q->cp_len; i++)
        rxy += rcp[i] * conj(rdelay[i]); // conj?
    rxy /= (float)(_q->cp_len);

    // TODO : push rxy into buffer?
    printf("|rxy| = %12.8f\n", cabsf(rxy));
}

