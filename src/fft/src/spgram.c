/*
 * Copyright (c) 2011 Joseph Gaeddert
 * Copyright (c) 2011 Virginia Polytechnic Institute & State University
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
    unsigned int nfft;      // fft length
    unsigned int overlap;   //
    float alpha;            // filter

    windowcf buffer;        // input buffer
    float complex * x;      // pointer to input array (allocated)
    float complex * X;      // output fft (allocated)
    float complex * w;      // tapering window
    float * psd;            // psd (allocated)
    fftplan p;              // fft plan

    unsigned int num_windows; // number of fft windows accumulated
    unsigned int index;     //
};

spgram spgram_create(unsigned int _nfft)
{
    // TODO : validate input

    spgram q = (spgram) malloc(sizeof(struct spgram_s));

    // input parameters
    q->nfft    = _nfft;
    q->overlap = q->nfft / 4;
    q->alpha   = 0.02f;

    q->buffer = windowcf_create(q->nfft);
    q->x = (float complex*) malloc((q->nfft)*sizeof(float complex));
    q->X = (float complex*) malloc((q->nfft)*sizeof(float complex));
    q->w = (float complex*) malloc((q->nfft)*sizeof(float complex));
    q->psd = (float*) malloc((q->nfft)*sizeof(float));

    q->p = fft_create_plan(q->nfft, q->x, q->X, FFT_FORWARD, 0);

    // initialize tapering window, scaled by FFT size
    unsigned int i;
    for (i=0; i<q->nfft; i++)
        q->w[i] = hamming(i,q->nfft) / (float)(q->nfft);

    q->num_windows = 0;
    q->index = 0;

    return q;
}

void spgram_destroy(spgram _q)
{
    windowcf_destroy(_q->buffer);

    free(_q->x);
    free(_q->X);
    free(_q->w);
    free(_q->psd);
    fft_destroy_plan(_q->p);
    free(_q);
}

void spgram_push(spgram _q,
                 float complex * _x,
                 unsigned int _n)
{
    // push/write samples
    //windowcf_write(_q->buffer, _x, _n);

    unsigned int i;
    for (i=0; i<_n; i++) {
        windowcf_push(_q->buffer, _x[i]);

        _q->index++;

        if (_q->index == _q->overlap) {
            unsigned int j;

            // reset counter
            _q->index = 0;

            // read buffer, copy to fft input (applying window)
            float complex * rc;
            windowcf_read(_q->buffer, &rc);
            for (i=0; i<_q->nfft; i++)
                _q->x[i] = rc[i] * _q->w[i];

            // execute fft
            fft_execute(_q->p);
            
            // accumulate output
            if (_q->num_windows == 0) {
                // first window; copy internally
                for (j=0; j<_q->nfft; j++)
                    _q->psd[j] = cabsf(_q->X[j]);
            } else {
                // 
                for (j=0; j<_q->nfft; j++)
                    _q->psd[j] = (1.0f - _q->alpha)*_q->psd[j] + _q->alpha*cabsf(_q->X[j]);
            }

            _q->num_windows++;
        }
    }
}

// execute spectral periodogram
//  _q      :   spectral periodogram object
//  _X      :   
void spgram_execute(spgram _q,
                    float * _X)
{
    // check to see if any transforms have been taken
    if (_q->num_windows == 0) {
        // copy zeros to output
        unsigned int i;
        for (i=0; i<_q->nfft; i++)
            _X[i] = 0.0f;

        return;
    }

    // copy shifted PSD contents to output
    unsigned int i;
    unsigned int k;
    unsigned int nfft_2 = _q->nfft / 2;
    for (i=0; i<_q->nfft; i++) {
        k = (i + nfft_2) % _q->nfft;
        _X[i] = 20*log10f(fabsf(_q->psd[k]));
    }
}


