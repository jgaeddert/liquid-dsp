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
// fft_create_plan.c
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

#define liquid_safe_free(q) { if (q != NULL) free(q); }

FFT(plan) FFT(_create_plan)(unsigned int _n,
                            TC * _x,
                            TC * _y,
                            int _dir,
                            int _flags)
{
    FFT(plan) p = (FFT(plan)) malloc(_n*sizeof(struct FFT(plan_s)));

    p->n = _n;
    p->x = _x;
    p->y = _y;
    p->flags = _flags;
    p->kind = LIQUID_FFT_DFT_1D;

    if (_dir == FFT_FORWARD)
        p->direction = FFT_FORWARD;
    else
        p->direction = FFT_REVERSE;

    // initialize arrays to NULL
    p->twiddle = NULL;
    p->index_rev = NULL;
    p->xr = NULL;
    p->yr = NULL;
    p->xc = NULL;
    p->yc = NULL;
    p->w  = NULL;

    p->is_radix2 = 0;   // false

    // check to see if _n is radix 2
    unsigned int i, d=0, m=0, t=p->n;
    for (i=0; i<8*sizeof(unsigned int); i++) {
        d += (t & 1);           // count bits, radix-2 if d==1
        if (!m && (t&1)) m = i; // count lagging zeros, effectively log2(n)
        t >>= 1;
    }

    // initialize twiddle factors, etc.
    if (_n <= FFT_SIZE_LUT ) {
        FFT(_init_lut)(p);
        p->execute = &FFT(_execute_lut);
    } else if (d==1) {
        // radix-2
        p->is_radix2 = 1;   // true
        p->m = m;
        FFT(_init_radix2)(p);
        p->execute = &FFT(_execute_radix2);
    } else {
        p->execute = &FFT(_execute_dft);
    }

    return p;
}

void FFT(_destroy_plan)(FFT(plan) _p)
{
    // safely free arrays
    liquid_safe_free(_p->twiddle);
    liquid_safe_free(_p->index_rev);

    // real even/odd DFTs
    liquid_safe_free(_p->xc);
    liquid_safe_free(_p->yc);
    liquid_safe_free(_p->w);

    free(_p);
}

FFT(plan) FFT(_create_plan_r2r_1d)(unsigned int _n,
                                   T * _x,
                                   T * _y,
                                   int _kind,
                                   int _flags)
{
    FFT(plan) p = (FFT(plan)) malloc(_n*sizeof(struct FFT(plan_s)));

    p->n  = _n;
    p->xr = _x;
    p->yr = _y;
    p->flags = _flags;

    // initialize all arrays to NULL
    p->twiddle = NULL;
    p->index_rev = NULL;
    p->xc = NULL;
    p->yc = NULL;
    p->w  = NULL;

    switch (_kind) {
    case FFT_REDFT00:
        // DCT-I
        p->kind = LIQUID_FFT_REDFT00;
        p->execute = &FFT(_execute_REDFT00);
        break;
    case FFT_REDFT10:
        // DCT-II
        p->kind = LIQUID_FFT_REDFT10;
        p->execute = &FFT(_execute_REDFT10);
        break;
    case FFT_REDFT01:
        // DCT-III
        p->kind = LIQUID_FFT_REDFT01;
        p->execute = &FFT(_execute_REDFT01);
        break;
    case FFT_REDFT11:
        // DCT-IV
        p->kind = LIQUID_FFT_REDFT11;
        p->execute = &FFT(_execute_REDFT11);
        break;
    default:
        fprintf(stderr,"error: fft_create_plan_r2r_1d(), invalid kind, %d\n", _kind);
        exit(1);
    }

    return p;
}

// create plan for (inverse) modified discrete cosine transform
//  _n      :   transform size
//  _x      :   
//  _y      :
//  _kind   :   FFT_MDCT or FFT_IMDCT
FFT(plan) FFT(_create_plan_mdct)(unsigned int _n,
                                 T * _x,
                                 T * _y,
                                 int _kind,
                                 int _flags)
{
    FFT(plan) p = (FFT(plan)) malloc(_n*sizeof(struct FFT(plan_s)));

    p->n  = _n;
    p->xr = _x;
    p->yr = _y;
    p->flags = _flags;

    p->kind = _kind;

    // initialize all arrays to NULL
    p->twiddle = NULL;
    p->index_rev = NULL;
    p->xr = NULL;
    p->yr = NULL;
    p->xc = NULL;
    p->yc = NULL;
    p->w  = (T*)malloc( (2*p->n)*sizeof(T) );

    // create window
    unsigned int i;
    for (i=0; i<2*p->n; i++) {
        // shaped pulse
        float t0 = sinf(M_PI/(2*p->n)*(i+0.5));
        p->w[i] = sinf(M_PI*0.5f*t0*t0);
    }

    switch (_kind) {
    case FFT_MDCT:
        // MDCT
        p->kind = LIQUID_FFT_MDCT;
        p->execute = &FFT(_execute_MDCT);
        break;
    case FFT_IMDCT:
        // IMDCT
        p->kind = LIQUID_FFT_IMDCT;
        p->execute = &FFT(_execute_IMDCT);
        break;
    default:
        fprintf(stderr,"error: fft_create_plan_mdct(), invalid kind, %d\n", _kind);
        exit(1);
    }

    return p;
}

// initialize twiddle factors using plain look-up table
void FFT(_init_lut)(FFT(plan) _p)
{
    unsigned int k, n, N = _p->n;
    _p->twiddle = (TC*) malloc(N*N*sizeof(TC));
    T phi, d = (_p->direction==FFT_FORWARD) ? -1 : 1;
    for (k=0; k<N; k++) {
        for (n=0; n<N; n++) {
            phi = 2*M_PI*d*((T)n)*((T)k) / (T) (N);
            _p->twiddle[k*N + n] = cexp(_Complex_I*phi);
        }   
    }   
}

void FFT(_init_radix2)(FFT(plan) _p)
{
    _p->index_rev = (unsigned int *) malloc((_p->n)*sizeof(unsigned int));
    unsigned int i;
    for (i=0; i<_p->n; i++)
        _p->index_rev[i] = reverse_index(i,_p->m);

    //for (i=0; i<_p->n; i++)
    //    printf("%3d -> %3d\n", i, _p->index_rev[i]);
}

// reverse _n-bit index _i
unsigned int reverse_index(unsigned int _i, unsigned int _n)
{
    unsigned int j=0, k;
    for (k=0; k<_n; k++) {
        j <<= 1;
        j |= ( _i & 1 );
        _i >>= 1;
    }

    return j;
}

// execute fft : simply calls internal function pointer
void FFT(_execute)(FFT(plan) _p)
{
    _p->execute(_p);
}

