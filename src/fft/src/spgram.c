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
// spgram (spectral periodogram)
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <complex.h>
#include "liquid.internal.h"

struct SPGRAM(_s) {
    // options
    unsigned int nfft;          // FFT length
    unsigned int window_len;    // window length

    WINDOW() buffer;            // input buffer
    TC * x;                     // pointer to input array (allocated)
    TC * X;                     // output fft (allocated)
    T  * w;                     // tapering window [size: window_len x 1]
    FFT_PLAN fft;               // fft plan

    // psd accumulation
    T * psd;
    unsigned int sample_counter;
    unsigned int num_transforms;
};

// create spgram object
//  _nfft       :   FFT size
//  _window     :   window coefficients [size: _window_len x 1]
//  _window_len :   window length
SPGRAM() SPGRAM(_create)(unsigned int _nfft,
                         float *      _window,
                         unsigned int _window_len)
{
    // validate input
    if (_nfft < 2) {
        fprintf(stderr,"error: spgram%s_create(), fft size must be at least 2\n", EXTENSION);
        exit(1);
    } else if (_window_len > _nfft) {
        fprintf(stderr,"error: spgram%s_create(), window size cannot exceed fft size\n", EXTENSION);
        exit(1);
    } else if (_window_len == 0) {
        fprintf(stderr,"error: spgram%s_create(), window size must be greater than zero\n", EXTENSION);
        exit(1);
    }

    // allocate memory for main object
    SPGRAM() q = (SPGRAM()) malloc(sizeof(struct SPGRAM(_s)));

    // set input parameters
    q->nfft       = _nfft;
    q->window_len = _window_len;

    // create FFT arrays, object
    q->x   = (TC*) malloc((q->nfft)*sizeof(TC));
    q->X   = (TC*) malloc((q->nfft)*sizeof(TC));
    q->psd = (T *) malloc((q->nfft)*sizeof(T ));
    q->fft = FFT_CREATE_PLAN(q->nfft, q->x, q->X, FFT_DIR_FORWARD, FFT_METHOD);

    // create buffer
    q->buffer = WINDOW(_create)(q->window_len);

    // allocate memory for window
    q->w = (T*) malloc((q->window_len)*sizeof(T));

    // scale by window magnitude, FFT size
    unsigned int i;
    float g = 0.0f;
    for (i=0; i<q->window_len; i++)
        g += _window[i] * _window[i];
    g = M_SQRT2 / ( sqrtf(g / q->window_len) * sqrtf((float)(q->nfft)) );

    // scale window and copy
    for (i=0; i<q->window_len; i++)
        q->w[i] = g * _window[i];
    
    // reset the spgram object
    SPGRAM(_reset)(q);

    // return new object
    return q;
}

// create spgram object using Kaiser-Bessel window
//  _nfft       :   FFT size
//  _window_len :   window length
SPGRAM() SPGRAM(_create_kaiser)(unsigned int _nfft,
                                unsigned int _window_len,
                                float        _beta)
{
    // validate input
    if (_nfft < 2) {
        fprintf(stderr,"error: spgram%s_create_kaiser(), fft size must be at least 2\n", EXTENSION);
        exit(1);
    } else if (_window_len > _nfft) {
        fprintf(stderr,"error: spgram%s_create_kaiser(), window size cannot exceed fft size\n", EXTENSION);
        exit(1);
    } else if (_window_len == 0) {
        fprintf(stderr,"error: spgram%s_create_kaiser(), window size must be greater than zero\n", EXTENSION);
        exit(1);
    } else if (_beta <= 0.0f) {
        fprintf(stderr,"error: spgram%s_create_kaiser(), beta must be greater than zero\n", EXTENSION);
        exit(1);
    }

    // initialize tapering window, scaled by window length size
    float * w = (float*) malloc((_window_len)*sizeof(float));
    unsigned int i;
    float mu = 0.0f;
    for (i=0; i<_window_len; i++)
        w[i] = kaiser(i, _window_len, _beta, mu);

    // create spgram object
    SPGRAM() q = SPGRAM(_create)(_nfft, w, _window_len);

    // free window buffer
    free(w);

    // return new object
    return q;
}

// create default spgram object (Kaiser-Bessel window)
SPGRAM() SPGRAM(_create_default)(unsigned int _nfft)
{
    // validate input
    if (_nfft < 2) {
        fprintf(stderr,"error: spgram%s_create_default(), fft size must be at least 2\n", EXTENSION);
        exit(1);
    }

    // return object
    unsigned int window_size = _nfft/2;
    float        beta        = 10.0f;
    return SPGRAM(_create_kaiser)(_nfft, window_size, beta);
}

// destroy spgram object
void SPGRAM(_destroy)(SPGRAM() _q)
{
    // free allocated memory
    free(_q->x);
    free(_q->X);
    free(_q->w);
    free(_q->psd);
    WINDOW(_destroy)(_q->buffer);
    FFT_DESTROY_PLAN(_q->fft);

    // free main object
    free(_q);
}

// resets the internal state of the spgram object
void SPGRAM(_reset)(SPGRAM() _q)
{
    // clear the window buffer
    WINDOW(_clear)(_q->buffer);

    // clear FFT input
    unsigned int i;
    for (i=0; i<_q->nfft; i++)
        _q->x[i] = 0.0f;

    // reset counters
    _q->num_transforms = 0;
    _q->sample_counter = 0;

    // clear PSD accumulation (set equal to unity, equal to zero dB)
    for (i=0; i<_q->nfft; i++)
        _q->psd[i] = 1;
}

// push a single sample into the spgram object
//  _q      :   spgram object
//  _x      :   input sample
void SPGRAM(_push)(SPGRAM() _q,
                   TI       _x)
{
    // push sample into internal window
    WINDOW(_push)(_q->buffer, _x);
}

// write a block of samples to the spgram object
//  _q      :   spgram object
//  _x      :   input buffer [size: _n x 1]
//  _n      :   input buffer length
void SPGRAM(_write)(SPGRAM()     _q,
                    TI *         _x,
                    unsigned int _n)
{
    // write a block of samples to the internal window
    WINDOW(_write)(_q->buffer, _x, _n);
}


// compute spectral periodogram output (complex values)
// from current buffer contents
//  _q      :   spgram object
//  _X      :   output complex spectrum [size: _nfft x 1]
void SPGRAM(_execute)(SPGRAM() _q,
                      TC *     _X)
{
    unsigned int i;

    // read buffer, copy to FFT input (applying window)
    // TODO: use SIMD extensions to speed this up
    TI * rc;
    WINDOW(_read)(_q->buffer, &rc);
    for (i=0; i<_q->window_len; i++)
        _q->x[i] = rc[i] * _q->w[i];

    // execute fft on _q->x and store result in _q->X
    FFT_EXECUTE(_q->fft);

    // copy result to output
    if (_X != NULL)
        memmove(_X, _q->X, _q->nfft*sizeof(TC));
}

// compute spectral periodogram output (fft-shifted values
// in dB) from current buffer contents
//  _q      :   spgram object
//  _X      :   output spectrum [size: _nfft x 1]
void SPGRAM(_execute_psd)(SPGRAM() _q,
                          T *      _X)
{
    // run internal transform
    SPGRAM(_execute)(_q, NULL);

    // compute magnitude in dB and run FFT shift
    unsigned int i;
    unsigned int nfft_2 = _q->nfft / 2;
    for (i=0; i<_q->nfft; i++) {
        unsigned int k = (i + nfft_2) % _q->nfft;

        _X[k] = 10*log10f( crealf( _q->X[i] * conjf(_q->X[i]) ) + 1e-16f);
    }
}

// accumulate power spectral density
//  _q      :   spgram object
//  _x      :   input buffer [size: _n x 1]
//  _alpha  :   auto-regressive memory factor, [0,1]
//  _n      :   input buffer length
void SPGRAM(_accumulate_psd)(SPGRAM()     _q,
                             TI *         _x,
                             float        _alpha,
                             unsigned int _n)
{
    // validate input
    if (_alpha < 0.0f || _alpha > 1.0f) {
        fprintf(stderr,"error: spgram%s_accumulate_psd(), alpha must be in (0,1)\n", EXTENSION);
        exit(1);
    }

    // push samples and run FFT at appropriate time
    unsigned int i;
    for (i=0; i<_n; i++) {
        // push sample
        WINDOW(_push)(_q->buffer, _x[i]);

        // increment counter
        _q->sample_counter++;

        // run FFT
        if (_q->sample_counter == _q->window_len/2) {
            // override alpha for first transform
            if (_q->num_transforms==0)
                _alpha = 1.0f;

            // execute transform
            SPGRAM(_execute)(_q, NULL);

            // accumulate squared magnitude response scaled by alpha
            unsigned int k;
            for (k=0; k<_q->nfft; k++)
                _q->psd[k] = (1.0f - _alpha)*_q->psd[k] + _alpha*crealf( _q->X[k] * conjf(_q->X[k]) );

            // reset counter
            _q->sample_counter = 0;

            // increment number of transforms taken
            _q->num_transforms++;
        }
    }
}

// write accumulated psd
//  _q      :   spgram object
//  _x      :   input buffer [size: _n x 1]
//  _n      :   input buffer length [size: _nfft x 1]
void SPGRAM(_write_accumulation)(SPGRAM() _q,
                                 T *      _x)
{
    unsigned int i;

#if 0
    // check number of transforms
    if (_q->num_transforms == 0)
        fprintf(stderr,"warning: spgram%s_write_accumulation(), no transforms taken yet\n", EXTENSION);
#endif

    // scale result by number of transforms and run fft shift
    unsigned int nfft_2 = _q->nfft / 2;
    //float        scale  = -10*log10f( (float)(_q->num_transforms) );
    for (i=0; i<_q->nfft; i++)
        _x[(i+nfft_2)%_q->nfft] = 10*log10f(_q->psd[i]);
}

// estimate spectrum on input signal
//  _q      :   spgram object
//  _x      :   input signal [size: _n x 1]
//  _n      :   input signal length
//  _psd    :   output spectrum, [size: _nfft x 1]
void SPGRAM(_estimate_psd)(SPGRAM()     _q,
                           TI *         _x,
                           unsigned int _n,
                           T *          _psd)
{
    // return if input size is zero
    if (_n == 0) {
        fprintf(stderr,"warning: spgram%s_estimate_psd(), input size is zero\n", EXTENSION);
        return;
    }

    // reset object
    SPGRAM(_reset)(_q);

    unsigned int i;
    unsigned int k;

    //
    unsigned int delay = _q->nfft / 4;
    if (delay == 0)
        delay = 1;

    // reset output array
    for (i=0; i<_q->nfft; i++)
        _psd[i] = 0.0f;

    // keep track of how many transforms have been taken
    unsigned int num_transforms = 0;

    //
    for (i=0; i<_n; i++) {
        // push signal into periodogram object one sample
        // at a time
        SPGRAM(_push)(_q, _x[i]);

        // take transform periodically, ensuring all samples
        // are taken into account
        if ( ((i+1)%delay)==0 || (i==_n-1)) {
            // run trasform
            SPGRAM(_execute)(_q, NULL);

            // accumulate power spectral density, taking fft shift
            for (k=0; k<_q->nfft; k++) {
                unsigned int p = (k + _q->nfft/2) % _q->nfft;
                _psd[p] += crealf(_q->X[k] * conjf(_q->X[k]));
            }

            // increment number of transforms taken
            num_transforms++;
        }
    }

    // at least one transform should have been taken
    assert(num_transforms > 0);
    
    // scale result by number of transforms and compute result in dB
    for (i=0; i<_q->nfft; i++)
        _psd[i] = 10*log10f( _psd[i] / (float)(num_transforms) );
}

