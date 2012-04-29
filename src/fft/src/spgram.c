/*
 * Copyright (c) 2011, 2012 Joseph Gaeddert
 * Copyright (c) 2011, 2012 Virginia Polytechnic
 *                          Institute & State University
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
// spgram (spectral periodogram)
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <complex.h>
#include "liquid.internal.h"

struct spgram_s {
    // options
    unsigned int nfft;          // FFT length
    unsigned int window_len;    // window length

    windowcf buffer;    // input buffer
    float complex * x;  // pointer to input array (allocated)
    float complex * X;  // output fft (allocated)
    float complex * w;  // tapering window [size: w x 1]
    FFT_PLAN fft;       // fft plan
};

// create spgram object
//  _nfft       :   FFT size
//  _window_len :   window length
spgram spgram_create(unsigned int _nfft,
                     unsigned int _window_len)
{
    // validate input
    if (_nfft < 2) {
        fprintf(stderr,"error: spgram_create_advanced(), fft size must be at least 2\n");
        exit(1);
    } else if (_window_len > _nfft) {
        fprintf(stderr,"error: spgram_create_advanced(), window size cannot exceed fft size\n");
        exit(1);
    }

    // allocate memory for main object
    spgram q = (spgram) malloc(sizeof(struct spgram_s));

    // set input parameters
    q->nfft       = _nfft;
    q->window_len = _window_len;

    // create FFT arrays, object
    q->x = (float complex*) malloc((q->nfft)*sizeof(float complex));
    q->X = (float complex*) malloc((q->nfft)*sizeof(float complex));
    q->fft = FFT_CREATE_PLAN(q->nfft, q->x, q->X, FFT_DIR_FORWARD, FFT_METHOD);

    // create buffer
    q->buffer = windowcf_create(q->window_len);

    // initialize tapering window, scaled by window length size
    // TODO : scale by window magnitude, FFT size as well
    q->w = (float complex*) malloc((q->window_len)*sizeof(float complex));
    unsigned int i;
    for (i=0; i<q->window_len; i++)
        q->w[i] = hamming(i, q->window_len) / (float)(q->window_len);

    // reset the spgram object
    spgram_reset(q);

    // return new object
    return q;
}

// destroy spgram object
void spgram_destroy(spgram _q)
{
    // free allocated memory
    free(_q->x);
    free(_q->X);
    free(_q->w);
    windowcf_destroy(_q->buffer);
    FFT_DESTROY_PLAN(_q->fft);

    // free main object
    free(_q);
}

// resets the internal state of the spgram object
void spgram_reset(spgram _q)
{
    // clear the window buffer
    windowcf_clear(_q->buffer);

    // clear FFT input
    unsigned int i;
    for (i=0; i<_q->nfft; i++)
        _q->x[i] = 0.0f;
}

// push samples into spgram object
//  _q      :   spgram object
//  _x      :   input buffer [size: _n x 1]
//  _n      :   input buffer length
void spgram_push(spgram          _q,
                 float complex * _x,
                 unsigned int    _n)
{
    // push/write samples
    windowcf_write(_q->buffer, _x, _n);
}


// compute spectral periodogram output
//  _q      :   spgram object
//  _X      :   output spectrum
void spgram_execute(spgram          _q,
                    float complex * _X)
{
    unsigned int i;

    // read buffer, copy to FFT input (applying window)
    float complex * rc;
    windowcf_read(_q->buffer, &rc);
    for (i=0; i<_q->window_len; i++)
        _q->x[i] = rc[i] * _q->w[i];

    // execute fft
    FFT_EXECUTE(_q->fft);

    // copy result to output
    memmove(_X, _q->X, _q->nfft*sizeof(float complex));
}

