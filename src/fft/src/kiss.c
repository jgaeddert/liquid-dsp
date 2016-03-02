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
// kiss.c : wrapper for kiss fft lib
//
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "liquid.internal.h"
#ifdef HAVE_LIBKISSFFT

#define KISS_FFT(name)           LIQUID_CONCAT(kiss_fft,name)

#define T                   float           /* primitive type */
#define TC                  float complex   /* primitive type (complex) */

typedef union {
    kiss_fft_cpx * cpx;
    TC * c;
} kiss_complex;

struct KISS_FFT(_plan_s)
{
    // common data
    kiss_fft_cfg kiss_cfg;
    kiss_complex x;             // input array pointer
    kiss_complex y;             // output array pointer
};

// create KISS_FFT plan, regular complex one-dimensional transform
//  _nfft   :   FFT size
//  _x      :   input array [size: _nfft x 1]
//  _y      :   output array [size: _nfft x 1]
//  _dir    :   fft direction: {KISS_FFT_FORWARD, KISS_FFT_BACKWARD}
//  _flags  :   fft method (not used)
KISS_FFT(_plan) KISS_FFT(_create_plan)(unsigned int _nfft,
                                       TC *         _x,
                                       TC *         _y,
                                       int          _dir,
                                       int          _flags)
{
    int is_inverse = (_dir == FFT_DIR_BACKWARD);
    KISS_FFT(_plan) q = (KISS_FFT(_plan)) malloc(sizeof(struct KISS_FFT(_plan_s)));

    q->kiss_cfg = kiss_fft_alloc(_nfft, is_inverse, NULL, NULL);

    // because kiss_complex relies on potentially underspecified behavior, make sure it works here
    TC test_complex[2];
    kiss_fft_cpx test_kiss_cpx[2];
    kiss_complex test_container;

    test_container.c = test_complex;
    test_container.cpx = test_kiss_cpx;

    test_complex[0] = 0.5f - _Complex_I * 0.5f;
    test_complex[1] = -0.75f + _Complex_I * 0.25f;

    assert(test_container.cpx[0].r == crealf(test_container.c[0]));
    assert(test_container.cpx[0].i == cimagf(test_container.c[0]));
    assert(test_container.cpx[1].r == crealf(test_container.c[1]));
    assert(test_container.cpx[1].i == cimagf(test_container.c[1]));

    q->x.c = _x;
    q->y.c = _y;

    return q;
}

// destroy KISS_FFT plan
void KISS_FFT(_destroy_plan)(KISS_FFT(_plan) _q)
{
    kiss_fft_free(_q->kiss_cfg);
    free(_q);
}

void KISS_FFT(_execute)(KISS_FFT(_plan) _q)
{
    kiss_fft(_q->kiss_cfg, _q->x.cpx, _q->y.cpx);
}
#endif // HAVE_LIBKISSFFT
