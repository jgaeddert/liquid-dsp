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

// initialize all internal arrays, etc. to null
void FFT(_init_null)(FFT(plan) _p)
{
    // 
    _p->twiddle = NULL;     // twiddle factors
    _p->index_rev = NULL;   // reverse indices (radix-2)

    // initialize input/output complex arrays (real FFTs, internal plan)
    _p->xc  = NULL;
    _p->yc  = NULL;

    // internal plan (real FFTs)
    _p->internal_plan = NULL;

    // modified discrete cosine transform (MDCT)
    _p->xrm = NULL;         // input buffer
    _p->yrm = NULL;         // output buffer
    _p->w   = NULL;         // window
}

FFT(plan) FFT(_create_plan)(unsigned int _n,
                            TC * _x,
                            TC * _y,
                            int _dir,
                            int _flags)
{
    // allocate plan and initialize all internal arrays to NULL
    FFT(plan) p = (FFT(plan)) malloc(sizeof(struct FFT(plan_s)));
    FFT(_init_null)(p);

    p->n = _n;
    p->x = _x;
    p->y = _y;
    p->flags = _flags;
    p->kind = LIQUID_FFT_DFT_1D;
    p->direction = (_dir == FFT_FORWARD) ? FFT_FORWARD : FFT_REVERSE;

    // check to see if transform size is radix 2
    p->is_radix2 = fft_is_radix2(p->n);

    // strip flags (see if method is forced)A
    p->method = LIQUID_FFT_METHOD_UNKNOWN;
    if (0) {
        // method is forced
    } else {
        // determine best method
        if (_n <= FFT_SIZE_LUT)
            p->method = LIQUID_FFT_METHOD_LUT;
        else if (p->is_radix2)
            p->method = LIQUID_FFT_METHOD_RADIX2;
        else
            p->method = LIQUID_FFT_METHOD_DFT;
    }

    // initialize twiddle factors, etc.
    switch ( p->method ) {
    case LIQUID_FFT_METHOD_LUT:
        FFT(_init_lut)(p);
        p->execute = &FFT(_execute_lut);
        break;
    case LIQUID_FFT_METHOD_RADIX2:
        // radix-2
        p->m = liquid_msb_index(p->n) - 1;  // m = log2(n)
        FFT(_init_radix2)(p);
        p->execute = &FFT(_execute_radix2);
        break;
    case LIQUID_FFT_METHOD_DFT:
        p->execute = &FFT(_execute_dft);
        break;
    default:
        fprintf(stderr,"error: fft_create_plan(), unkown/unsupported method %d\n", p->method);
        exit(1);
    }

    return p;
}

FFT(plan) FFT(_create_plan_r2r_1d)(unsigned int _n,
                                   T * _x,
                                   T * _y,
                                   int _kind,
                                   int _flags)
{
    // allocate plan and initialize all internal arrays to NULL
    FFT(plan) p = (FFT(plan)) malloc(sizeof(struct FFT(plan_s)));
    FFT(_init_null)(p);

    p->n  = _n;
    p->xr = _x;
    p->yr = _y;
    p->kind = _kind;
    p->flags = _flags;
    p->method = LIQUID_FFT_METHOD_DFT;

    // check to see if transform size is radix 2
    p->is_radix2 = fft_is_radix2(p->n);

    // TODO : break of this switch into manageably-sized pieces

    switch (p->kind) {
    case FFT_REDFT00:
        // DCT-I
        p->execute = &FFT(_execute_REDFT00);
        break;
    case FFT_REDFT10:
        // DCT-II
        if (p->is_radix2) {
            // create internal plan
            p->xc = (TC*) malloc(_n*sizeof(TC));
            p->yc = (TC*) malloc(_n*sizeof(TC));
            p->internal_plan = FFT(_create_plan)(_n, p->xc, p->yc, FFT_FORWARD, _flags);
            p->execute = &FFT(_execute_REDFT10_fftn);
        } else {
            // compute regular DCT
            p->execute = &FFT(_execute_REDFT10);
        }
        break;
    case FFT_REDFT01:
        // DCT-III
        p->execute = &FFT(_execute_REDFT01);
#if 0
        // create internal plan
        p->xc = (TC*) malloc(_n*sizeof(TC));
        p->yc = (TC*) malloc(_n*sizeof(TC));
        p->internal_plan = FFT(_create_plan)(_n, p->xc, p->yc, FFT_REVERSE, _flags);
#endif
        break;
    case FFT_REDFT11:
        // DCT-IV
        if (p->is_radix2) {
            // create internal plan
            p->xc = (TC*) malloc(4*p->n*sizeof(TC));
            p->yc = (TC*) malloc(4*p->n*sizeof(TC));
            p->internal_plan = FFT(_create_plan)(4*p->n, p->xc, p->yc, FFT_FORWARD, _flags);
            p->execute = &FFT(_execute_REDFT11_fft4n);
        } else {
            // compute regular DCT
            p->execute = &FFT(_execute_REDFT11);
        }
        break;
    case FFT_RODFT00:
        // DST-I
        p->execute = &FFT(_execute_RODFT00);
        break;
    case FFT_RODFT10:
        // DST-II
        p->execute = &FFT(_execute_RODFT10);
        break;
    case FFT_RODFT01:
        // DST-III
        p->execute = &FFT(_execute_RODFT01);
        break;
    case FFT_RODFT11:
        // DST-IV
        p->execute = &FFT(_execute_RODFT11);
        break;
    default:
        fprintf(stderr,"error: fft_create_plan_r2r_1d(), invalid kind, %d\n", p->kind);
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
    // allocate plan and initialize all internal arrays to NULL
    FFT(plan) p = (FFT(plan)) malloc(sizeof(struct FFT(plan_s)));
    FFT(_init_null)(p);

    p->n  = _n;
    p->xr = _x;
    p->yr = _y;
    p->flags = _flags;

    p->kind = _kind;

    // allocate memory for MDCT window
    p->w  = (T*)malloc( (2*p->n)*sizeof(T) );

    // create window
    unsigned int i;
    for (i=0; i<2*p->n; i++) {
#if 0
        // shaped pulse
        float t0 = sinf(M_PI/(2*p->n)*(i+0.5));
        p->w[i] = sinf(M_PI*0.5f*t0*t0);
#else
        p->w[i] = 1.0f;
#endif
    }

    switch (p->kind) {
    case FFT_MDCT:
        // MDCT
        if (0) {
            // create internal plan
            p->xrm = (T*) malloc(_n*sizeof(T));
            p->yrm = (T*) malloc(_n*sizeof(T));
            p->internal_plan = FFT(_create_plan_r2r_1d)(p->n, p->xrm, p->yrm, FFT_REDFT11, 0);
            p->execute = &FFT(_execute_MDCT_REDFT11);
        } else {
            p->execute = &FFT(_execute_MDCT);
        }
        break;
    case FFT_IMDCT:
        // IMDCT
        if (0) {
            // create internal plan
            p->xrm = (T*) malloc(_n*sizeof(T));
            p->yrm = (T*) malloc(_n*sizeof(T));
            p->internal_plan = FFT(_create_plan_r2r_1d)(p->n, p->xrm, p->yrm, FFT_REDFT11, 0);
            p->execute = &FFT(_execute_IMDCT_REDFT11);
        } else {
            p->execute = &FFT(_execute_IMDCT);
        }
        break;
    default:
        fprintf(stderr,"error: fft_create_plan_mdct(), invalid kind, %d\n", p->kind);
        exit(1);
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

    // (I)MDCT
    liquid_safe_free(_p->w);
    liquid_safe_free(_p->xrm);
    liquid_safe_free(_p->yrm);

    // destroy internal plan (used for real DFTs)
    if (_p->internal_plan != NULL)
        FFT(_destroy_plan)(_p->internal_plan);

    // free main object memory
    free(_p);
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
        _p->index_rev[i] = fft_reverse_index(i,_p->m);

    //for (i=0; i<_p->n; i++)
    //    printf("%3d -> %3d\n", i, _p->index_rev[i]);
}

// execute fft : simply calls internal function pointer
void FFT(_execute)(FFT(plan) _p)
{
    _p->execute(_p);
}

