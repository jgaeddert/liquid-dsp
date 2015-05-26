/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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

struct ASGRAM(_s) {
    unsigned int nfft;          // transform size
    SPGRAM()     periodogram;   // spectral periodogram object
    TC *         X;             // spectral periodogram output
    float *      psd;           // power spectral density

    float        levels[10];    // threshold for signal levels
    char         levelchar[10]; // characters representing levels
    unsigned int num_levels;    // number of levels
    float        scale;         // dB per division
    float        offset;        // dB offset (max)
};

// create asgram object with size _nfft
ASGRAM() ASGRAM(_create)(unsigned int _nfft)
{
    // validate input
    if (_nfft < 2) {
        fprintf(stderr,"error: asgram%s_create(), fft size must be at least 2\n", EXTENSION);
        exit(1);
    }

    // create main object
    ASGRAM() q = (ASGRAM()) malloc(sizeof(struct ASGRAM(_s)));

    q->nfft = _nfft;

    // allocate memory for PSD estimate
    q->X   = (TC *   ) malloc((q->nfft)*sizeof(TC)   );
    q->psd = (float *) malloc((q->nfft)*sizeof(float));

    // create spectral periodogram object
    unsigned int window_len = q->nfft;
    float beta = 10.0f;
    q->periodogram = SPGRAM(_create_kaiser)(q->nfft, window_len, beta);

    // power spectral density levels
    q->num_levels = 10;
    q->levelchar[9] = '#';
    q->levelchar[8] = 'M';
    q->levelchar[7] = 'N';
    q->levelchar[6] = '&';
    q->levelchar[5] = '*';
    q->levelchar[4] = '+';
    q->levelchar[3] = '-';
    q->levelchar[2] = ',';
    q->levelchar[1] = '.';
    q->levelchar[0] = ' ';

    ASGRAM(_set_scale)(q, 0.0f, 10.0f);

    return q;
}

// destroy asgram object
void ASGRAM(_destroy)(ASGRAM() _q)
{
    // destroy spectral periodogram object
    SPGRAM(_destroy)(_q->periodogram);

    // free PSD estimate array
    free(_q->X);
    free(_q->psd);

    // free main object memory
    free(_q);
}

// resets the internal state of the asgram object
void ASGRAM(_reset)(ASGRAM() _q)
{
    SPGRAM(_reset)(_q->periodogram);
}

// set scale and offset for spectrogram
//  _q      :   asgram object
//  _offset :   signal offset level [dB]
//  _scale  :   signal scale [dB]
void ASGRAM(_set_scale)(ASGRAM() _q,
                        float    _offset,
                        float    _scale)
{
    if (_scale <= 0.0f) {
        fprintf(stderr,"ASGRAM(_set_scale)(), scale must be greater than zero\n");
        exit(1);
    }

    _q->offset = _offset;
    _q->scale  = _scale;

    unsigned int i;
    for (i=0; i<_q->num_levels; i++)
        _q->levels[i] = _q->offset + i*_q->scale;
}

// push a single sample into the asgram object
//  _q      :   asgram object
//  _x      :   input buffer [size: _n x 1]
//  _n      :   input buffer length
void ASGRAM(_push)(ASGRAM() _q,
                   TI       _x)
{
    // push sample into internal spectral periodogram
    SPGRAM(_push)(_q->periodogram, _x);
}

// write a block of samples to the asgram object
//  _q      :   asgram object
//  _x      :   input buffer [size: _n x 1]
//  _n      :   input buffer length
void ASGRAM(_write)(ASGRAM()     _q,
                    TI *         _x,
                    unsigned int _n)
{
    // write samples to internal spectral periodogram
    SPGRAM(_write)(_q->periodogram, _x, _n);
}

// compute spectral periodogram output from current buffer contents
//  _q          :   ascii spectrogram object
//  _ascii      :   character buffer [size: 1 x n]
//  _peakval    :   value at peak (returned value)
//  _peakfreq   :   frequency at peak (returned value)
void ASGRAM(_execute)(ASGRAM()  _q,
                      char *    _ascii,
                      float *   _peakval,
                      float *   _peakfreq)
{
    // execute spectral periodogram
    SPGRAM(_execute)(_q->periodogram, _q->X);

    // compute PSD magnitude and apply FFT shift
    unsigned int i;
    for (i=0; i<_q->nfft; i++)
        _q->psd[i] = 10*log10f(cabsf(_q->X[(i + _q->nfft/2)%_q->nfft]));

    unsigned int j;
    for (i=0; i<_q->nfft; i++) {
        // find peak
        if (i==0 || _q->psd[i] > *_peakval) {
            *_peakval = _q->psd[i];
            *_peakfreq = (float)(i) / (float)(_q->nfft) - 0.5f;
        }

        // determine ascii level (which character to use)
#if 0
        for (j=0; j<_q->num_levels-1; j++) {
            if ( _q->psd[i] > ( _q->offset - j*(_q->scale)) )
                break;
        }
        _ascii[i] = _q->levelchar[j];
#else
        _ascii[i] = _q->levelchar[0];
        for (j=0; j<_q->num_levels; j++) {
            if ( _q->psd[i] > _q->levels[j] )
                _ascii[i] = _q->levelchar[j];
        }
#endif
    }

    // append null character to end of string
    //_ascii[i] = '\0';
}

// compute spectral periodogram output from current buffer
// contents and print standard format to stdout
void ASGRAM(_print)(ASGRAM() _q)
{
    float maxval;
    float maxfreq;
    char ascii[_q->nfft+1];
    ascii[_q->nfft] = '\0'; // append null character to end of string
        
    // execute the spectrogram
    ASGRAM(_execute)(_q, ascii, &maxval, &maxfreq);

    // print the spectrogram to stdout
    printf(" > %s < pk%5.1f dB [%5.2f]\n", ascii, maxval, maxfreq);
}

