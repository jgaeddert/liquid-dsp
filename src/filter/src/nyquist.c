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
//
//

#include <string.h>
#include <stdio.h>
#include <math.h>

// FIR matched filter root mean-square inter-symbol interference
float fir_mf_rms_isi(unsigned int _n, unsigned int _k, float *_h)
{
    float rms=0.0f;

    // compute filter auto-correlation
    unsigned int hxx_len = 2*_n - 1;
    float hxx[hxx_len];

    if (_n%2)
        printf("n is even\n");
    else
        printf("n is odd\n");

    // compute auto-correlation
    //vect_rxxf(_h, _n, hxx);

    unsigned int i, p=0, xxx=0;
    for (i=xxx; i<hxx_len; i+= _k) {
        rms += hxx[i] * hxx[i];
        p++;
    }

    return sqrtf(rms/p);
}

// compute autocorrelation
//    _x    : input vector
//    _n    : length of _x
//    _rxx  : output vector, length (2*_n)-1
void vect_rxxf(float *_x, unsigned int _n, float *_rxx)
{
    unsigned int i,j;
    unsigned int rxx_len=(2*_n)-1;
    for (i=0; i<rxx_len; i++)
        _rxx[i] = 0.0f;

    // TODO fix this logic
    for (i=0; i<_n; i++) {
        for (j=0; j<i; j++) {
            _rxx[i] += _x[i] * _x[j];
        }
    }
}
