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
// fftfilt : finite impulse response (FIR) filter using fast Fourier
//           transforms (FFTs)
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// defined:
//  FFTFILT()       name-mangling macro
//  T               coefficients type
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

// fftfilt object structure
struct FFTFILT(_s) {
    TC * h;             // filter coefficients array [size; h_len x 1]
    unsigned int h_len; // filter length
    unsigned int n;     // input/output block size

    // internal memory arrays
    // TODO: make TI/TO type, but ensuring complex
    // TODO: use special format for fftfilt_rrrf type
    TI * time_buf;      // time buffer [size: 2*n x 1]
    TI * freq_buf;      // freq buffer [size: 2*n x 1]
    TI * H;             // FFT of filter coefficients [size: 2*n x 1]
    TI * w;             // overlap array [size: n x 1]

    // FFT objects
    FFT_PLAN fft;       // FFT object (forward)
    FFT_PLAN ifft;      // FFT object (inverse)

    TC scale;           // output scaling factor
};

// create FFT-based FIR filter using external coefficients
//  _h      : filter coefficients [size: _h_len x 1]
//  _h_len  : filter length, _h_len > 0
//  _n      : block size = nfft/2, at least _h_len-1
FFTFILT() FFTFILT(_create)(TC *         _h,
                           unsigned int _h_len,
                           unsigned int _n)
{
    // validate input
    if (_h_len == 0) {
        fprintf(stderr,"error: fftfilt_%s_create(), filter length must be greater than zero\n",
                EXTENSION_FULL);
        exit(1);
    } else if (_n < _h_len-1) {
        fprintf(stderr,"error: fftfilt_%s_create(), block length must be greater than _h_len-1 (%u)\n",
                EXTENSION_FULL,
                _h_len-1);
        exit(1);
    }

    // create filter object and initialize
    FFTFILT() q = (FFTFILT()) malloc(sizeof(struct FFTFILT(_s)));
    q->h_len    = _h_len;
    q->n        = _n;

    // copy filter coefficients
    q->h = (TC *) malloc((q->h_len)*sizeof(TC));
    memmove(q->h, _h, _h_len*sizeof(TC));

    // allocate internal memory arrays
    q->time_buf = (TI *) malloc((2*q->n)* sizeof(TI)); // time buffer
    q->freq_buf = (TI *) malloc((2*q->n)* sizeof(TI)); // frequency buffer
    q->H        = (TI *) malloc((2*q->n)* sizeof(TI)); // FFT{ h }
    q->w        = (TI *) malloc((  q->n)* sizeof(TI)); // delay buffer

    // create internal FFT objects
    q->fft  = FFT_CREATE_PLAN(2*q->n, q->time_buf, q->freq_buf, FFT_DIR_FORWARD,  FFT_METHOD);
    q->ifft = FFT_CREATE_PLAN(2*q->n, q->freq_buf, q->time_buf, FFT_DIR_BACKWARD, FFT_METHOD);

    // compute FFT of filter coefficients

    // set default scaling
    q->scale = 1;

    // reset filter state (clear buffer)
    FFTFILT(_reset)(q);

    // return object
    return q;
}

// destroy object, freeing all internally-allocated memory
void FFTFILT(_destroy)(FFTFILT() _q)
{
    // free internal arrays
    free(_q->h);                // filter coefficients
    free(_q->time_buf);         // buffer (time domain)
    free(_q->freq_buf);         // buffer (frequency domain)
    free(_q->H);                // frequency response of filter coefficients
    free(_q->w);                // output window buffer

    // destroy FFT objects
    FFT_DESTROY_PLAN(_q->fft);  // forward transform
    FFT_DESTROY_PLAN(_q->ifft); // reverse transform

    // free main object
    free(_q);
}

// reset internal state of filter object
void FFTFILT(_reset)(FFTFILT() _q)
{
    // reset overlap window
    unsigned int i;
    for (i=0; i<_q->n; i++)
        _q->w[i] = 0;
}

// print filter object internals (taps, buffer)
void FFTFILT(_print)(FFTFILT() _q)
{
    printf("fftfilt_%s: [h_len=%u, n=%u]\n", EXTENSION_FULL, _q->h_len, _q->n);
    unsigned int i;
    unsigned int n = _q->h_len;
    for (i=0; i<n; i++) {
        printf("  h(%3u) = ", i+1);
        PRINTVAL_TC(_q->h[n-i-1],%12.8f);
        printf("\n");
    }

    // print scaling
    printf("  scale = ");
    PRINTVAL_TC(_q->scale,%12.8f);
    printf("\n");
}

// set output scaling for filter
void FFTFILT(_set_scale)(FFTFILT() _q,
                         TC        _scale)
{
    _q->scale = _scale;
}

// execute the filter on internal buffer and coefficients
//  _q      : filter object
//  _x      : pointer to input data array  [size: _n x 1]
//  _y      : pointer to output data array [size: _n x 1]
void FFTFILT(_execute)(FFTFILT() _q,
                       TI *      _x,
                       TO *      _y)
{
    unsigned int i;

    // copy input
    memmove(_q->time_buf, _x, _q->n*sizeof(TI));

    // pad end of time-domain buffer with zeros
#if 0
    memset(&_q->time_buf[_q->n], 0, _q->n*sizeof(TI));
#else
    for (i=0; i<_q->n; i++)
        _q->time_buf[_q->n + i] = 0;
#endif

    // run forward transform
    FFT_EXECUTE(_q->fft);

    // compute inner product between FFT{ _x } and FFT{ H }
    // TODO: use SIMD vector extensions for this
    for (i=0; i<2*_q->n; i++)
        _q->freq_buf[i] *= _q->H[i];

    // compute inverse transform
    FFT_EXECUTE(_q->ifft);

    // copy output summed with buffer and scaled
    for (i=0; i<_q->n; i++)
        _y[i] = (_q->freq_buf[i] + _q->w[i]) * _q->scale;

    // copy buffer
    memmove(_q->w, &_q->freq_buf[_q->n], _q->n*sizeof(TO));
}

// return length of filter object's internal coefficients
unsigned int FFTFILT(_get_length)(FFTFILT() _q)
{
    return _q->h_len;
}

