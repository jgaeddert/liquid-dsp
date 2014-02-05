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
// fct_exeucte.c
//
// FCT : Fast (Discrete) Cosine Transforms
//

#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

// 
// DCT-I
//

// 
// DCT-II (regular 'dct')
//
void FFT(_execute_REDFT10_fftn)(FFT(plan) _p)
{
    unsigned int i;
    unsigned int r = _p->n % 2;
    unsigned int L = (_p->n - r)/2;

    // precondition fft
    for (i=0; i<L; i++) {
        _p->xc[i]         = _p->xr[2*i+0];
        _p->xc[_p->n-i-1] = _p->xr[2*i+1];
    }
    // check for odd condition
    if (r==1)
        _p->xc[L] = _p->xr[2*L];

    // execute fft, storing result in _p->yc
    FFT(_execute)(_p->internal_plan);

    // post-condition output
    for (i=0; i<_p->n; i++)
        _p->yr[i] = 2.0f*crealf(_p->yc[i]*cexpf(-_Complex_I*0.5f*M_PI*i/((float)_p->n)));
}

// 
// DCT-III (regular 'idct')
//
void FFT(_execute_REDFT01_fftn)(FFT(plan) _p)
{
    unsigned int i;
    unsigned int r = _p->n % 2;
    unsigned int L = (_p->n - r)/2;

    // precondition fft
    for (i=0; i<_p->n; i++)
        _p->xc[i] = _p->xr[i] * cexpf(_Complex_I*0.5f*M_PI*i/((float)_p->n));

    // execute fft, storing result in _p->yc
    FFT(_execute)(_p->internal_plan);

    // post-condition output
    for (i=0; i<L; i++) {
        _p->yr[2*i  ] = 2.0f * crealf( _p->yc[i] );
        _p->yr[2*i+1] = 2.0f * crealf( _p->yc[_p->n-i-1] );
    }
    // check for odd condition
    if (r==1)
        _p->yr[2*L] = 2.0f * crealf( _p->yc[L] );
}

// 
// DCT-IV
//
void FFT(_execute_REDFT11_fft4n)(FFT(plan) _p)
{
    // NOTE: this method is only faster if n >= 64 and is radix-2

    unsigned int i;
    // precondition fft
    for (i=0; i<_p->n; i++) {
        _p->xc[0*(_p->n) + i] =  _p->xr[i];
        _p->xc[1*(_p->n) + i] = -_p->xr[_p->n-i-1];
        _p->xc[2*(_p->n) + i] = -_p->xr[i];
        _p->xc[3*(_p->n) + i] =  _p->xr[_p->n-i-1];
    }

    // execute fft, storing result in _p->yc
    FFT(_execute)(_p->internal_plan);

    // post-condition output
    for (i=0; i<_p->n; i++) {
        float theta = 0.5 * M_PI * ((float)i + 0.5) / ((float)_p->n);
        _p->yr[i] = 0.5f*crealf(_p->yc[2*i+1]*cexpf(-_Complex_I*theta));
    }
}
