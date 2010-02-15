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
// Bessel filter design
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "liquid.internal.h"

#define LIQUID_DEBUG_BESSEL_PRINT   0

// forward declarations

void cfpoly_bessel(unsigned int _n, int * _p);

void cfpoly_bessel_roots(unsigned int _n, float complex * _roots);

// ****************************************

void bessel_azpkf(unsigned int _n,
                  float complex * _za,
                  float complex * _pa,
                  float complex * _ka)
{
    unsigned int r = _n%2;
    unsigned int L = (_n - r)/2;
    
    unsigned int i;
    unsigned int k=0;
    // compute poles (roots to Bessel polynomial)
    for (i=0; i<L; i++) {
        _pa[k++] = 0.0f;
    }
}

void cfpoly_bessel(unsigned int _n, int * _p)
{
    unsigned int k;
    for (k=0; k<_n; k++) {
        float t0 = liquid_lngammaf((float)(2*_n-k)+1);
        float t1 = liquid_lngammaf((float)(_n-k)  +1);
        float t2 = liquid_lngammaf((float)(k)     +1);
        float t3 = 1<<(_n-k);

        _p[k] = (int) ( expf(t0 - t1 -t2)/t3 );
#if 0
        printf("  p[%3u,%3u] = %d\n", k, _n, _p[k]);
        printf("    t0 : %12.8f\n", t0);
        printf("    t1 : %12.8f\n", t1);
        printf("    t2 : %12.8f\n", t2);
        printf("    t3 : %12.8f\n", t3);
#endif
    }
}

void cfpoly_bessel_roots(unsigned int _n,
                         float complex * _roots)
{
    if (_n < 4) {
        float complex p[_n];
        cfpoly_bessel(_n,p);
        cfpoly_findroots(p,_n,_roots);
    } else {
        float m0 = -0.668861023825672*_n + 0.352940768662957;
        float m1 = 1.0f / (1.6013579390149844*_n - 0.0429146801453954);

        unsigned int i;
        unsigned int r = _n%2;
        unsigned int L = (_n-r)/2;
        float ri, rq;
        for (i=0; i<_n; i++) {
            rq = (i - L - r + 0.5)*1.778f;
            ri = m0 * m1*rq*rq;

            _roots[i] = ri + _Complex_I*rq;
        }
    }
}

