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

// create FFT plan for regular DFT
//  _nfft   :   FFT size
//  _x      :   input array [size: _nfft x 1]
//  _y      :   output array [size: _nfft x 1]
//  _dir    :   fft direction: {FFT_FORWARD, FFT_REVERSE}
//  _method :   fft method
FFT(plan) FFT(_create_plan_rader)(unsigned int _nfft,
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
    q->method    = LIQUID_FFT_METHOD_RADER;

    q->execute   = FFT(_execute_rader);

    // initialize 'prime' factors for mixed-radix algorithm for FFT of
    // size nfft-1
    FFT(_mixed_radix_init_factors)(q, q->nfft-1);

    // allocate and initialize twiddle factors for mixed-radix
    // transforms of size nfft-1
    // NOTE: double the number of twiddle factors and compute both
    //       forward and reverse permutations
    q->twiddle = (TC *) malloc(2 * (q->nfft-1) * sizeof(TC));
    
    unsigned int i;
    for (i=0; i<q->nfft-1; i++) {
        T phi = 2*M_PI*i / (T)(q->nfft-1);

        q->twiddle[i]           = cexpf(-_Complex_I*phi);   // FFT
        q->twiddle[i+q->nfft-1] = cexpf( _Complex_I*phi);   // IFFT
    }

    // compute primitive root of nfft
    unsigned int g = liquid_primitive_root_prime(q->nfft);

    // create and initialize sequence
    q->seq = (unsigned int *)malloc((q->nfft-1)*sizeof(unsigned int));
    for (i=0; i<q->nfft-1; i++)
        q->seq[i] = liquid_modpow(g, i+1, q->nfft);
    
    // compute DFT of sequence { exp(-j*2*pi*g^i/nfft }, size: nfft-1
    // NOTE: R[0] = -1, |R[k]| = sqrt(nfft) for k != 0
    TC * r = (TC*)malloc((q->nfft-1)*sizeof(TC));  // temporary buffer
    q->R   = (TC*)malloc((q->nfft-1)*sizeof(TC));
    T d = (q->direction == FFT_FORWARD) ? -1.0 : 1.0;
    for (i=0; i<q->nfft-1; i++)
        r[i] = cexpf(_Complex_I*d*2*M_PI*q->seq[i]/(T)(q->nfft));
    FFT(_run)(q->nfft-1, r, q->R, FFT_FORWARD, 0);
    free(r);    // free temporary buffer

    // return main object
    return q;
}

// destroy FFT plan
void FFT(_destroy_plan_rader)(FFT(plan) _q)
{
    // free data specific to Rader's algorithm
    free(_q->seq);  // sequence
    free(_q->R);    // pre-computed transform of exp(j*2*pi*seq)

    // free data specific to mixed-radix transforms
    free(_q->twiddle);

    // free main object memory
    free(_q);
}

// execute Rader's algorithm
void FFT(_execute_rader)(FFT(plan) _q)
{
    unsigned int i;

    // compute DFT of permuted sequence, size: nfft-1
    TC * xp = (TC*)malloc((_q->nfft-1)*sizeof(TC));
    TC * Xp = (TC*)malloc((_q->nfft-1)*sizeof(TC));
    for (i=0; i<_q->nfft-1; i++) {
        // reverse sequence
        unsigned int k = _q->seq[_q->nfft-1-i-1];
        xp[i] = _q->x[k];
    }
    // call mixed-radix function (FFT)
    // equivalent to: FFT(_run)(_q->nfft-1, xp, Xp, FFT_FORWARD, 0);
    FFT(_mixed_radix_cycle)(xp, Xp, _q->twiddle, _q->nfft-1, 0, 1, _q->m_vect, _q->p_vect);

    // compute inverse FFT of product
    for (i=0; i<_q->nfft-1; i++)
        Xp[i] *= _q->R[i];
    // call mixed-radix function (IFFT)
    // equivalent to: FFT(_run)(_q->nfft-1, Xp, xp, FFT_REVERSE, 0);
    // NOTE: inverse FFT is facilitated by passing second set of
    //       conjugated twiddle factors
    FFT(_mixed_radix_cycle)(Xp, xp, _q->twiddle+_q->nfft-1, _q->nfft-1, 0, 1, _q->m_vect, _q->p_vect);

    // set DC value
    _q->y[0] = 0.0f;
    for (i=0; i<_q->nfft; i++)
        _q->y[0] += _q->x[i];

    // reverse permute result, scale, and add offset x[0]
    for (i=0; i<_q->nfft-1; i++) {
        unsigned int k = _q->seq[i];

        _q->y[k] = xp[i] / (T)(_q->nfft-1) + _q->x[0];
    }

    // free internal memory
    free(xp);
    free(Xp);
}

