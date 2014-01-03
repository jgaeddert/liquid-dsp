/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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
#include <assert.h>

#include <complex.h>
#include "liquid.internal.h"

struct spgram_s {
    // options
    unsigned int nfft;          // FFT length
    unsigned int window_len;    // window length

    windowcf buffer;            // input buffer
    float complex * x;          // pointer to input array (allocated)
    float complex * X;          // output fft (allocated)
    float *         w;          // tapering window [size: window_len x 1]
    FFT_PLAN fft;               // fft plan

    // psd accumulation
    float * psd;
    unsigned int sample_counter;
    unsigned int num_transforms;
};

// create spgram object
//  _nfft       :   FFT size
//  _window_len :   window length
spgram spgram_create(unsigned int _nfft,
                     float *      _window,
                     unsigned int _window_len)
{
    // validate input
    if (_nfft < 2) {
        fprintf(stderr,"error: spgram_create(), fft size must be at least 2\n");
        exit(1);
    } else if (_window_len > _nfft) {
        fprintf(stderr,"error: spgram_create(), window size cannot exceed fft size\n");
        exit(1);
    } else if (_window_len == 0) {
        fprintf(stderr,"error: spgram_create(), window size must be greater than zero\n");
        exit(1);
    }

    // allocate memory for main object
    spgram q = (spgram) malloc(sizeof(struct spgram_s));

    // set input parameters
    q->nfft       = _nfft;
    q->window_len = _window_len;

    // create FFT arrays, object
    q->x   = (float complex*) malloc((q->nfft)*sizeof(float complex));
    q->X   = (float complex*) malloc((q->nfft)*sizeof(float complex));
    q->psd = (float *)        malloc((q->nfft)*sizeof(float));
    q->fft = FFT_CREATE_PLAN(q->nfft, q->x, q->X, FFT_DIR_FORWARD, FFT_METHOD);

    // create buffer
    q->buffer = windowcf_create(q->window_len);

    // allocate memory for window and copy
    q->w = (float*) malloc((q->window_len)*sizeof(float));
    memmove(q->w, _window, _window_len*sizeof(float));

    // scale by window magnitude, FFT size
    unsigned int i;
    float g = 0.0f;
    for (i=0; i<q->window_len; i++)
        g += q->w[i] * q->w[i];
    g = 1.0f / ( sqrtf(g / q->window_len) * sqrtf((float)(q->nfft)) );
    for (i=0; i<q->window_len; i++)
        q->w[i] *= g;

    // reset the spgram object
    spgram_reset(q);

    // return new object
    return q;
}

// create spgram object using Kaiser-Bessel window
//  _nfft       :   FFT size
//  _window_len :   window length
spgram spgram_create_kaiser(unsigned int _nfft,
                            unsigned int _window_len,
                            float        _beta)
{
    // validate input
    if (_nfft < 2) {
        fprintf(stderr,"error: spgram_create_kaiser(), fft size must be at least 2\n");
        exit(1);
    } else if (_window_len > _nfft) {
        fprintf(stderr,"error: spgram_create_kaiser(), window size cannot exceed fft size\n");
        exit(1);
    } else if (_window_len == 0) {
        fprintf(stderr,"error: spgram_create_kaiser(), window size must be greater than zero\n");
        exit(1);
    } else if (_beta <= 0.0f) {
        fprintf(stderr,"error: spgram_create_kaiser(), beta must be greater than zero\n");
        exit(1);
    }

    // initialize tapering window, scaled by window length size
    float * w = (float*) malloc((_window_len)*sizeof(float));
    unsigned int i;
    float mu = 0.0f;
    for (i=0; i<_window_len; i++)
        w[i] = kaiser(i, _window_len, _beta, mu);

    // create spgram object
    spgram q = spgram_create(_nfft, w, _window_len);

    // free window buffer
    free(w);

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
    free(_q->psd);
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

    // clear PSD accumulation
    _q->num_transforms = 0;
    _q->sample_counter          = 0;
    for (i=0; i<_q->nfft; i++)
        _q->psd[i] = 0.0f;
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
    // TODO: use SIMD extensions to speed this up
    float complex * rc;
    windowcf_read(_q->buffer, &rc);
    for (i=0; i<_q->window_len; i++)
        _q->x[i] = rc[i] * _q->w[i];

    // execute fft
    FFT_EXECUTE(_q->fft);

    // copy result to output
    if (_X != NULL)
        memmove(_X, _q->X, _q->nfft*sizeof(float complex));
}

// accumulate power spectral density
//  _q      :   spgram object
//  _x      :   input buffer [size: _n x 1]
//  _n      :   input buffer length
void spgram_accumulate_psd(spgram                 _q,
                           liquid_float_complex * _x,
                           unsigned int           _n)
{
    // push samples and run FFT at appropriate time
    unsigned int i;
    for (i=0; i<_n; i++) {
        // push sample
        windowcf_push(_q->buffer, _x[i]);

        // increment counter
        _q->sample_counter++;

        // run FFT
        if (_q->sample_counter == _q->window_len/2) {
            // reset counter
            _q->sample_counter = 0;

            // execute transform
            spgram_execute(_q, NULL);

            // accumulate squared magnitude response
            unsigned int k;
            for (k=0; k<_q->nfft; k++)
                _q->psd[k] += crealf( _q->X[k] * conjf(_q->X[k]) );

            // increment number of transforms taken
            _q->num_transforms++;
        }
    }
}

// write accumulated psd
//  _q      :   spgram object
//  _x      :   input buffer [size: _n x 1]
//  _n      :   input buffer length [size: _nfft x 1]
void spgram_write_accumulation(spgram  _q,
                               float * _x)
{
    unsigned int i;

    // scale result by number of transforms and run fft shift
    unsigned int nfft_2 = _q->nfft / 2;
    for (i=0; i<_q->nfft; i++)
        _x[(i+nfft_2)%_q->nfft] = 10*log10f( _q->psd[i] / (float)(_q->num_transforms) );

}

// estimate spectrum on input signal
//  _q      :   spgram object
//  _x      :   input signal [size: _n x 1]
//  _n      :   input signal length
//  _psd    :   output spectrum, [size: _nfft x 1]
void spgram_estimate_psd(spgram                 _q,
                         liquid_float_complex * _x,
                         unsigned int           _n,
                         float *                _psd)
{
    unsigned int i;
    unsigned int k;

    //
    unsigned int delay = _q->nfft / 4;
    if (delay == 0)
        delay = 1;

    // reset output array
    for (i=0; i<_q->nfft; i++)
        _psd[i] = 0.0f;

    // return if input size is zero
    if (_n == 0)
        return;

    // keep track of how many transforms have been taken
    unsigned int num_transforms = 0;

    // temporary array for output
    float complex * X = (float complex*) malloc(_q->nfft * sizeof(float complex));

    //
    for (i=0; i<_n; i++) {
        // push signal into periodogram object one sample
        // at a time
        spgram_push(_q, &_x[i], 1);

        // take transform periodically, ensuring all samples
        // are taken into account
        if ( ((i+1)%delay)==0 || (i==_n-1)) {
            spgram_execute(_q, X);
            for (k=0; k<_q->nfft; k++)
                _psd[k] += crealf(X[k] * conjf(X[k]));

            //
            num_transforms++;
        }
    }

    // at least one transform should have been taken
    assert(num_transforms > 0);
    
    // scale result by number of transforms
    for (i=0; i<_q->nfft; i++)
        _psd[i] /= (float)(num_transforms);

    // free allocated memory
    free(X);
}

