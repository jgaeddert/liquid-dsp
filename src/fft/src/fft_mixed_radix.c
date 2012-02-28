/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010, 2011 Virginia Polytechnic
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
// fft_mixed_radix.c : definitions for mixed-radix transforms using
//                     the Cooley-Tukey algorithm
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "liquid.internal.h"

#define FFT_DEBUG_MIXED_RADIX 0

// create FFT plan for regular DFT
//  _nfft   :   FFT size
//  _x      :   input array [size: _nfft x 1]
//  _y      :   output array [size: _nfft x 1]
//  _dir    :   fft direction: {FFT_FORWARD, FFT_REVERSE}
//  _method :   fft method
FFT(plan) FFT(_create_plan_mixed_radix)(unsigned int _nfft,
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
    q->method    = LIQUID_FFT_METHOD_MIXED_RADIX;

    q->execute   = FFT(_execute_mixed_radix);

    // find 'prime' factors
    unsigned int i;
    unsigned int k;
    unsigned int n = q->nfft;
    unsigned int num_factors = 0;

    do {
        for (k=2; k<=n; k++) {
            if ( (n%k)==0 ) {
                // prefer radix 4 over 2 if possible
                if ( (k==2) && (n%4)==0 )
                    k = 4;

                n /= k;
                q->p_vect[num_factors] = k;
                q->m_vect[num_factors] = n;
                num_factors++;
                break;
            }
        }
    } while (n > 1 && num_factors < FFT_MAX_FACTORS);

    // NOTE: this is extremely unlikely as the worst case is
    //       nfft=2^MAX_FACTORS in which case we will probably run out
    //       of memory first
    if (num_factors == FFT_MAX_FACTORS) {
        fprintf(stderr,"error: could not factor %u with %u factors\n", q->nfft, FFT_MAX_FACTORS);
        exit(1);
    }

#if FFT_DEBUG_MIXED_RADIX
    printf("factors of %u:\n", q->nfft);
    for (i=0; i<num_factors; i++)
        printf("  p=%3u, m=%3u\n", q->p_vect[i], q->m_vect[i]);
#endif

    // initialize twiddle factors, indices for mixed-radix transforms
    q->twiddle = (TC *) malloc(q->nfft * sizeof(TC));
    
    T d = (q->direction == FFT_FORWARD) ? -1.0 : 1.0;
    for (i=0; i<q->nfft; i++)
        q->twiddle[i] = cexpf(_Complex_I*d*2*M_PI*(T)i / (T)(q->nfft));

    return q;
}

// destroy FFT plan
void FFT(_destroy_plan_mixed_radix)(FFT(plan) _q)
{
    // free data specific to mixed-radix transforms
    free(_q->twiddle);

    // free main object memory
    free(_q);
}

// execute mixed-radix FFT
void FFT(_execute_mixed_radix)(FFT(plan) _q)
{
    // call mixed-radix function
    FFT(_mixed_radix_cycle)(_q->x, _q->y, _q->twiddle, _q->nfft, 0, 1, _q->m_vect, _q->p_vect);

}

// FFT mixed-radix butterfly for 2-point DFT
//  _x          :   input/output buffer pointer [size: _nfft x 1]
//  _twiddle    :   pre-computed twiddle factors [size: _nfft x 1]
//  _nfft       :   original FFT size
//  _stride     :   output stride
//  _m          :   number of FFTs to compute
//
// NOTES : the butterfly decimates in time, storing the output as
//         contiguous samples in the same buffer.
void FFT(_mixed_radix_bfly2)(TC *         _x,
                             TC *         _twiddle,
                             unsigned int _nfft,
                             unsigned int _stride,
                             unsigned int _m)
{
    unsigned int n;
    unsigned int twiddle_index = 0;
    for (n=0; n<_m; n++) {
        // strip input values
        TC x0 = _x[n];
        TC x1 = _x[n+_m];

        // compute 2-point DFT, using appropriate twiddles
        // x0 ---- y0
        //    \ /
        //     X
        //    / \
        // x1 ---- y1
        TC t = x1*_twiddle[twiddle_index];
        _x[n+_m] = _x[n] - t;
        _x[n]    = _x[n] + t;

        // update twiddle index
        twiddle_index += _stride;
    }
}

// FFT mixed-radix butterfly
//  _x          :   input/output buffer pointer [size: _nfft x 1]
//  _twiddle    :   pre-computed twiddle factors [size: _nfft x 1]
//  _nfft       :   original FFT size
//  _stride     :   output stride
//  _m          :   number of FFTs to compute
//  _p          :   generic (small) FFT size
//
// NOTES : the butterfly decimates in time, storing the output as
//         contiguous samples in the same buffer.
void FFT(_mixed_radix_bfly)(TC *         _x,
                            TC *         _twiddle,
                            unsigned int _nfft,
                            unsigned int _stride,
                            unsigned int _m,
                            unsigned int _p)
{
#if FFT_DEBUG_MIXED_RADIX
    printf("  bfly: stride=%3u, m=%3u, p=%3u\n", _stride, _m, _p);
#endif

    // create temporary buffer the size of the FFT
    TC * x_tmp = (TC *) malloc(_p*sizeof(TC));

    unsigned int i;
    unsigned int k;

    unsigned int n;
    for (n=0; n<_m; n++) {
#if FFT_DEBUG_MIXED_RADIX
        printf("    u=%u\n", n);
#endif

        // copy input to temporary buffer
        for (i=0; i<_p; i++)
            x_tmp[i] = _x[n + i*_m];
        
        // compute DFT, applying appropriate twiddle factors
        unsigned int twiddle_base = n;
        for (i=0; i<_p; i++) {
#if FFT_DEBUG_MIXED_RADIX
            printf("      ----\n");
#endif
            TC y = x_tmp[0];
            unsigned int twiddle_index = 0;
            for (k=1; k<_p; k++) {
                twiddle_index = (twiddle_index + _stride*twiddle_base) % _nfft;
#if FFT_DEBUG_MIXED_RADIX
                printf("      twiddle_index = %3u > %12.8f + j%12.8f, %12.8f + j%12.8f\n", twiddle_index, crealf(_twiddle[twiddle_index]), cimagf(_twiddle[twiddle_index]), crealf(x_tmp[k]), cimagf(x_tmp[k]));
#endif

                y += x_tmp[k] * _twiddle[twiddle_index];
            }
            // increment twiddle twiddle base
            twiddle_base += _m;

            // store output
            _x[n + i*_m] = y;
#if FFT_DEBUG_MIXED_RADIX
            printf("      y = %12.6f + j%12.6f\n", crealf(y), cimagf(y));
#endif
        }
    }

    // free temporary buffer
    free(x_tmp);
}

// FFT mixed-radix recursive function...
//  _x          :   constant input pointer [size: _nfft x 1]
//  _y          :   output pointer
//  _twiddle    :   pre-computed twiddle factors [size: _nfft x 1]
//  _nfft       :   original FFT size
//  _xoffset    :   input buffer offset
//  _xstride    :   input buffer stride
//  _m_vect     :   array of radix values [size: num_factors x 1]
//  _p_vect     :   array of DFT values [size: num_factors x 1]
void FFT(_mixed_radix_cycle)(TC *            _x,
                             TC *            _y,
                             TC *            _twiddle,
                             unsigned int    _nfft,
                             unsigned int    _xoffset,
                             unsigned int    _xstride,
                             unsigned int *  _m_vect,
                             unsigned int *  _p_vect)
{
    // de-reference factors and pop values off the top
    unsigned int m = _m_vect[0];    // radix
    unsigned int p = _p_vect[0];    // DFT size

    // increment factor pointers
    _m_vect++;
    _p_vect++;
    
#if FFT_DEBUG_MIXED_RADIX
    printf("fftmr_cycle:    offset=%3u, stride=%3u, p=%3u, m=%3u\n", _xoffset, _xstride, p, m);
#endif

    unsigned int i;
    if ( m == 1 ) {
        // copy data to output buffer
        for (i=0; i<p; i++)
            _y[i] = _x[_xoffset + _xstride*i];

    } else {
        // call fftmr_cycle() recursively, effectively computing
        // p DFTs each of size m samples, decimating the time
        // input by _xstride
        for (i=0; i<p; i++) {
            FFT(_mixed_radix_cycle)(_x,                    // input buffer (does not change)
                                    _y + i*m,              // increment output buffer by block size
                                    _twiddle,              // twiddle factors (no change)
                                    _nfft,                 // original FFT size (no change)
                                    _xoffset + _xstride*i, // input offset (increased by _xstride)
                                    _xstride*p,            // input stride (scaled by radix)
                                    _m_vect,               // array of radix values (length reduced by one)
                                    _p_vect);              // array of DFT values (length reduced by one)
        }
    }

    // run m-point DFT
    switch (p) {
    case 2:  FFT(_mixed_radix_bfly2)(_y, _twiddle, _nfft, _xstride, m); break;
    default: FFT(_mixed_radix_bfly)(_y, _twiddle, _nfft, _xstride, m, p);
    }
}
                      
