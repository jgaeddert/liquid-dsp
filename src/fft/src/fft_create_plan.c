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
// fft
//

#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

void fft_destroy_plan(fftplan _p)
{
    free(_p->twiddle);
    free(_p->index_rev);
    free(_p);
}

fftplan fft_create_plan(unsigned int _n,
                        TC * _x,
                        TC * _y,
                        int _dir,
                        int _method)
{
    fftplan p = (fftplan) malloc(_n*sizeof(struct fftplan_s));

    p->n = _n;
    p->x = _x;
    p->y = _y;
    p->method = _method;
    p->kind = LIQUID_FFT_DFT_1D;

    if (_dir == FFT_FORWARD)
        p->direction = FFT_FORWARD;
    else
        p->direction = FFT_REVERSE;

    p->twiddle = NULL;
    p->index_rev = NULL;

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
        fft_init_lut(p);
        p->execute = &fft_execute_lut;
    } else if (d==1) {
        // radix-2
        p->is_radix2 = 1;   // true
        p->m = m;
        fft_init_radix2(p);
        p->execute = &fft_execute_radix2;
    } else {
        p->execute = &fft_execute_dft;
    }

    return p;
}


fftplan fft_create_plan_r2r_1d(unsigned int _n,
                               T * _x,
                               T * _y,
                               int _kind,
                               int _method)
{
    fftplan p = (fftplan) malloc(_n*sizeof(struct fftplan_s));

    p->n  = _n;
    p->xr = _x;
    p->yr = _y;
    p->method = _method;
    switch (_kind) {
    case FFT_REDFT00:   p->kind = LIQUID_FFT_REDFT00;   break;
    case FFT_REDFT01:   p->kind = LIQUID_FFT_REDFT01;   break;
    case FFT_REDFT10:   p->kind = LIQUID_FFT_REDFT10;   break;
    case FFT_REDFT11:   p->kind = LIQUID_FFT_REDFT11;   break;
    default:
        fprintf(stderr,"error: fft_create_plan_r2r_1d(), invalid kind, %d\n", _kind);
        exit(1);
    }

    fprintf(stderr,"error: fft_create_plan_r2f_1d(), DCT not yet supported!\n");
    exit(1);

    return p;
}

// initialize twiddle factors using plain look-up table
void fft_init_lut(fftplan _p)
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

void fft_init_radix2(fftplan _p)
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

void fft_execute(fftplan _p)
{
    _p->execute(_p);
}

