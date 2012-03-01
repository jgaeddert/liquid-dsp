/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011, 2012 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2011, 2012 Virginia Polytechnic
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
// fft_rader.c : definitions for transforms of prime length using
//               Rader's algorithm
//
// References:
//  [Rader:1968] Charles M. Rader, "Discrete Fourier Transforms When
//      the Number of Data Samples Is Prime," Proceedings of the IEEE,
//      vol. 56, number 6, pp. 1107--1108, June 1968
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "liquid.internal.h"

#define FFT_DEBUG_RADER 0

// create FFT plan
//  _nfft   :   FFT size
//  _x      :   input array [size: _nfft x 1]
//  _y      :   output array [size: _nfft x 1]
//  _dir    :   fft direction: {FFT_FORWARD, FFT_REVERSE}
//  _method :   fft method
FFT(plan) FFT(_create_plan_rader_radix2)(unsigned int _nfft,
                                         TC *         _x,
                                         TC *         _y,
                                         int          _dir,
                                         int          _flags)
{
    // allocate plan and initialize all internal arrays to NULL
    FFT(plan) q = (FFT(plan)) malloc(sizeof(struct FFT(plan_s)));

    q->nfft      = _nfft;
    q->x         = _x;
    q->y         = _y;
    q->flags     = _flags;
    q->kind      = LIQUID_FFT_DFT_1D;
    q->direction = (_dir == FFT_FORWARD) ? FFT_FORWARD : FFT_REVERSE;
    q->method    = LIQUID_FFT_METHOD_RADER_RADIX2;

    q->execute   = FFT(_execute_rader_radix2);

    unsigned int i;

    // compute primitive root of nfft
    unsigned int g = liquid_primitive_root_prime(q->nfft);

    // create and initialize sequence
    q->seq = (unsigned int *)malloc((q->nfft-1)*sizeof(unsigned int));
    for (i=0; i<q->nfft-1; i++)
        q->seq[i] = liquid_modpow(g, i+1, q->nfft);

    // compute larger FFT length greater than 2*nfft-4
    // NOTE: while any length greater than 2*nfft-4 will work, use
    //       nfft_prime = 2 ^ nextpow2( 2*nfft - 4 ) to enable
    //       radix-2 transform
    unsigned int m=0;
    q->nfft_prime = (2*q->nfft-4)-1;
    while (q->nfft_prime > 0) {
        q->nfft_prime >>= 1;
        m++;
    }
    q->nfft_prime = 1 << m;
    //printf("nfft_prime = %u\n", q->nfft_prime);
    // assert(nfft_prime > 2*nfft-4)
    
    // compute DFT of sequence { exp(-j*2*pi*g^i/nfft }, size: nfft_prime
    // NOTE: R[0] = -1, |R[k]| = sqrt(nfft) for k != 0
    TC * r = (TC*)malloc(q->nfft_prime*sizeof(TC));     // temporary buffer
    q->R   = (TC*)malloc(q->nfft_prime*sizeof(TC));
    T d = (q->direction == FFT_FORWARD) ? -1.0 : 1.0;
    for (i=0; i<q->nfft_prime; i++)
        r[i] = cexpf(_Complex_I*d*2*M_PI*q->seq[i%(q->nfft-1)]/(T)(q->nfft));
    FFT(_run)(q->nfft_prime, r, q->R, FFT_FORWARD, 0);
    free(r);    // free temporary buffer

    // return main object
    return q;
}

// destroy FFT plan
void FFT(_destroy_plan_rader_radix2)(FFT(plan) _q)
{
    // free data specific to Rader's algorithm
    free(_q->seq);  // sequence
    free(_q->R);    // pre-computed transform of exp(j*2*pi*seq)

    // free data specific to radix-2 transform
    //free(_q->twiddle);

    // free main object memory
    free(_q);
}

// execute Rader's algorithm
void FFT(_execute_rader_radix2)(FFT(plan) _q)
{
    unsigned int i;

    // compute nfft_prime-length DFT of permuted sequence with
    // nfft_prime-nfft+1 zeros inserted after first element
    TC * xp = (TC*)malloc(_q->nfft_prime*sizeof(TC));
    TC * Xp = (TC*)malloc(_q->nfft_prime*sizeof(TC));
    xp[0] = _q->x[ _q->seq[_q->nfft-2] ];
    for (i=0; i<_q->nfft_prime-_q->nfft+1; i++)
        xp[i+1] = 0.0f;
    for (i=1; i<_q->nfft-1; i++) {
        // reverse sequence
        unsigned int k = _q->seq[_q->nfft-1-i-1];
        xp[i+_q->nfft_prime-_q->nfft+1] = _q->x[k];
    }
    // xp should be: { x[s[end]], 0, 0, 0, ...., 0, x[s[end-1]], x[s[end-2]], ... , x[s[0]] }
    // call radix-2 function (FFT)
    FFT(_run)(_q->nfft_prime, xp, Xp, FFT_FORWARD, 0);

    // compute inverse FFT of product
    // compute nfft_prime-length inverse FFT of product
    for (i=0; i<_q->nfft_prime; i++)
        Xp[i] *= _q->R[i];
    // call radix-2 function (IFFT)
    FFT(_run)(_q->nfft_prime, Xp, xp, FFT_REVERSE, 0);

    // set DC value
    _q->y[0] = 0.0f;
    for (i=0; i<_q->nfft; i++)
        _q->y[0] += _q->x[i];

    // reverse permute result, scale, and add offset x[0]
    for (i=0; i<_q->nfft-1; i++) {
        unsigned int k = _q->seq[i];

        _q->y[k] = xp[i] / (T)(_q->nfft_prime) + _q->x[0];
    }

    // free internal memory
    free(xp);
    free(Xp);
}

