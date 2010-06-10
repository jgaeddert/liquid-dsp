/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
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
// asgram (ASCII spectrogram)
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <complex.h>
#include "liquid.internal.h"

struct asgram_s {
    float complex * x;  // pointer to input array
    float complex * y;  // output fft (allocated)
    float * psd;        // psd (allocated)
    unsigned int n;     // fft length
    fftplan p;          // fft plan

    char levelchar[6];
    unsigned int num_levels;
    float scale;    // dB per division
    float offset;   // dB offset (max)
};

asgram asgram_create(float complex * _x,
                     unsigned int _n)
{
    asgram q = (asgram) malloc(sizeof(struct asgram_s));

    q->n = _n;
    q->x = _x;
    q->y = (float complex*) malloc((q->n)*sizeof(float complex));
    q->psd = (float*) malloc((q->n)*sizeof(float));

    q->p = fft_create_plan(q->n, q->x, q->y, FFT_FORWARD, 0);
    q->num_levels = 6;
    q->levelchar[0] = 'M';
    q->levelchar[1] = '#';
    q->levelchar[2] = '+';
    q->levelchar[3] = '-';
    q->levelchar[4] = '.';
    q->levelchar[5] = ' ';

    q->scale = 10.0f;
    q->offset = 20.0f;

    return q;
}

void asgram_set_scale(asgram _q, float _scale)
{
    _q->scale = _scale;
}

void asgram_set_offset(asgram _q, float _offset)
{
    _q->offset = _offset;
}

void asgram_destroy(asgram _q)
{
    free(_q->y);
    free(_q->psd);
    fft_destroy_plan(_q->p);
    free(_q);
}

// execute ascii spectrogram
//  _q          :   ascii spectrogram object
//  _ascii      :   character buffer [size: 1 x n]
//  _peakval    :   value at peak (returned value)
//  _peakfreq   :   frequency at peak (returned value)
void asgram_execute(asgram _q,
                    char * _ascii,
                    float * _peakval,
                    float * _peakfreq)
{
    // copy x and execute fft plan
    fft_execute(_q->p);

    // fftshift
    fft_shift(_q->y, _q->n);

    // compute PSD magnitude
    unsigned int i;
    for (i=0; i<_q->n; i++)
        _q->psd[i] = 20*log10f(cabsf(_q->y[i]));

    unsigned int j;
    for (i=0; i<_q->n; i++) {
        // find peak
        if (i==0 || _q->psd[i] > *_peakval) {
            *_peakval = _q->psd[i];
            *_peakfreq = (float)(i) / (float)(_q->n) - 0.5f;
        }

        // determine ascii level (which character to use)
        for (j=0; j<_q->num_levels-1; j++) {
            if ( _q->psd[i] > ( _q->offset - j*(_q->scale)) )
                break;
        }
        _ascii[i] = _q->levelchar[j];
    }

    // append null character to end of string
    //_ascii[i] = '\0';
}


