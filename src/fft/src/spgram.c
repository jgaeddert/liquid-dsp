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

    windowcf w;             // input window
    float complex * x;      // pointer to input array (allocated)
    float complex * X;      // output fft (allocated)
    float * psd;            // psd (allocated)
    fftplan p;              // fft plan

    unsigned int num_windows; // number of fft windows accumulated
    unsigned int index;     //
};

spgram spgram_create(unsigned int _nfft)
{
    // TODO : validate input

    spgram q = (spgram) malloc(sizeof(struct spgram_s));

    q->nfft    = _nfft;
    q->overlap = q->nfft / 4;

    q->w = windowcf_create(q->nfft);
    q->x = (float complex*) malloc((q->nfft)*sizeof(float complex));
    q->X = (float complex*) malloc((q->nfft)*sizeof(float complex));
    q->psd = (float*) malloc((q->nfft)*sizeof(float));

    q->p = fft_create_plan(q->nfft, q->x, q->X, FFT_FORWARD, 0);

    q->num_windows = 0;
    q->index = 0;

    return q;
}

void spgram_destroy(spgram _q)
{
    windowcf_destroy(_q->w);

    free(_q->x);
    free(_q->X);
    free(_q->psd);
    fft_destroy_plan(_q->p);
    free(_q);
}

void spgram_push(spgram _q,
                 float complex * _x,
                 unsigned int _n)
{
    // push/write samples
    //windowcf_write(_q->w, _x, _n);

    unsigned int i;
    for (i=0; i<_n; i++) {
        windowcf_push(_q->w, _x[i]);

        _q->index++;

        if (_q->index == _q->overlap) {
            // reset counter
            _q->index = 0;

            // read buffer, copy to fft input
            float complex * rc;
            windowcf_read(_q->w, &rc);
            memmove(_q->x, rc, _q->nfft*sizeof(float complex));

            // execute fft
            fft_execute(_q->p);
            
            // accumulate output
            unsigned int j;
            if (_q->num_windows == 0) {
                // first window; copy internally
                for (j=0; j<_q->nfft; j++)
                    _q->psd[i] = cabsf(_q->X[i]);
            } else {
                // 
                for (j=0; j<_q->nfft; j++)
                    _q->psd[i] = (1.0f - _q->alpha) + _q->alpha*cabsf(_q->X[i]);
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
    // copy shifted PSD contents to output
    unsigned int i;
    unsigned int k;
    unsigned int nfft_2 = _q->nfft / 2;
    for (i=0; i<_q->nfft; i++) {
        k = (i + nfft_2) % _q->nfft;
        _X[i] = 20*log10f(cabsf(_q->psd[k]));
    }
}


