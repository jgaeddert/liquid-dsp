/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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
// fft : inefficient but effective
//

#include <math.h>
#include "fft_internal.h"

void fft_execute(fftplan _p)
{
    if (_p->n <= FFT_SIZE_LUT)
        fft_execute_lut(_p);
    else if (_p->is_radix2)
        fft_execute_radix2(_p);
    else
        fft_execute_dft(_p);
}

void fft_execute_dft(fftplan _p)
{
    unsigned int k, n, N=_p->n;
    double phi, d = (_p->direction==FFT_FORWARD) ? -1 : 1;
    for (k=0; k<N; k++) {
        _p->y[k] = 0.0f;
        for (n=0; n<N; n++) {
            phi = 2*M_PI*d*((double)n)*((double)k) / (double) (N);
            _p->y[k] += _p->x[n] * cexp(_Complex_I*phi);
        }
    }
}

void fft_execute_lut(fftplan _p)
{
    unsigned int k, n, N=_p->n;
    for (k=0; k<N; k++) {
        _p->y[k] = 0.0f;
        for (n=0; n<N; n++) {
            // _p->y[k] = dot_prod_cc(_p->x, &_p->twiddle[k*N]);
            _p->y[k] += _p->x[n] * _p->twiddle[k*N + n];
        }
    }
}

void fft_execute_radix2(fftplan _p)
{
    // swap values
    unsigned int i,j,k;
    for (i=0; i<_p->n; i++)
        _p->y[i] = _p->x[ _p->index_rev[i] ];

    float complex t, yp, *y=_p->y;
    float phi, d_phi;
    unsigned int n1=0, n2=1;
    for (i=0; i<_p->m; i++) {
        n1 = n2;
        n2 *= 2;

        d_phi = (_p->direction == FFT_FORWARD) ? -2*M_PI/n2 : 2*M_PI/n2;
        phi = 0;

        for (j=0; j<n1; j++) {
            t = cexpf(_Complex_I*phi);
            phi += d_phi;

            for (k=j; k<_p->n; k+=n2) {
                yp = y[k+n1]*t;
                y[k+n1] = y[k] - yp;
                y[k] += yp;
            }
        }
    }
}

