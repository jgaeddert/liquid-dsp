/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2013 Joseph Gaeddert
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
    unsigned int nfft;  // transform size
    spgram periodogram; // spectral periodogram object
    float complex * X;  // spectral periodogram output
    float * psd;        // power spectral density

    char levelchar[6];
    unsigned int num_levels;
    float scale;    // dB per division
    float offset;   // dB offset (max)
};

asgram asgram_create(unsigned int _nfft)
{
    // validate input
    if (_nfft < 2) {
        fprintf(stderr,"error: asgram_create(), fft size must be at least 2\n");
        exit(1);
    }

    asgram q = (asgram) malloc(sizeof(struct asgram_s));

    q->nfft = _nfft;

    // allocate memory for PSD estimate
    q->X   = (float complex *) malloc((q->nfft)*sizeof(float complex));
    q->psd = (float *)         malloc((q->nfft)*sizeof(float));

    // create spectral periodogram object
    unsigned int window_len = q->nfft;
    float beta = 10.0f;
    q->periodogram = spgram_create_kaiser(q->nfft, window_len, beta);

    // power spectral density levels
    q->num_levels = 6;
    q->levelchar[0] = 'M';
    q->levelchar[1] = '#';
    q->levelchar[2] = '+';
    q->levelchar[3] = '-';
    q->levelchar[4] = '.';
    q->levelchar[5] = ' ';

    q->scale  = 10.0f;
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
    // destroy spectral periodogram object
    spgram_destroy(_q->periodogram);

    // free PSD estimate array
    free(_q->X);
    free(_q->psd);

    // free main object memory
    free(_q);
}

// push samples into asgram object
//  _q      :   asgram object
//  _x      :   input buffer [size: _n x 1]
//  _n      :   input buffer length
void asgram_push(asgram          _q,
                 float complex * _x,
                 unsigned int    _n)
{
    // push samples into internal spectral periodogram
    spgram_push(_q->periodogram, _x, _n);
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
    // execute spectral periodogram
    spgram_execute(_q->periodogram, _q->X);

    // compute PSD magnitude and apply FFT shift
    unsigned int i;
    for (i=0; i<_q->nfft; i++)
        _q->psd[i] = 20*log10f(cabsf(_q->X[(i + _q->nfft/2)%_q->nfft]));

    unsigned int j;
    for (i=0; i<_q->nfft; i++) {
        // find peak
        if (i==0 || _q->psd[i] > *_peakval) {
            *_peakval = _q->psd[i];
            *_peakfreq = (float)(i) / (float)(_q->nfft) - 0.5f;
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


