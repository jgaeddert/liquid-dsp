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
// binlinear z-transforms
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

// bilinear z-transform using zeros, poles, gain
//
//            (s-z[0])*(s-z[1])*...*(s-z[nz-1])
// H(s) = k * ---------------------------------
//            (s-p[0])*(s-p[1])*...*(s-z[np-1])
void bilinear_zpk(float complex * _z,
                  unsigned int _nz,
                  float complex * _p,
                  unsigned int _np,
                  float _k,
                  float * _bd,
                  unsigned int _nbd,
                  float * _ad,
                  unsigned int _nad);

// bilinear z-transform using polynomial expansion in numerator and
// denominator
//
//          b[0] + b[1]*s + ... + b[nb]*s^(nb-1)
// H(s) =   ------------------------------------
//          a[0] + a[1]*s + ... + a[na]*s^(na-1)
void bilinear_nd(float complex * _b,
                 unsigned int _b_order,
                 float complex * _a,
                 unsigned int _a_order,
                 float * _bd,
                 unsigned int _nbd,
                 float * _ad,
                 unsigned int _nad)
{
    // ...
    unsigned int nb = _b_order+1;
    unsigned int na = _a_order+1;
    float complex polyb[nb];
    float complex polya[na];

    unsigned int i;
    for (i=0; i<nb; i++) polyb[i] = 0;
    for (i=0; i<na; i++) polya[i] = 0;

    // temporary polynomial: (1 + 1/z)^(k) * (1 - 1/z)^(n-k)
    int poly_1pz[na];

    // compute denominator
    for (i=0; i<na; i++) {
        poly_binomial_expand_pm(na,i,poly_1pz);
        unsigned int j;
        for (j=0; j<na; j++)
            polya[j] += _a[i]*poly_1pz[j];
    }

    // for now assume numerator has zero terms...
    poly_binomial_expand(na,poly_1pz);
    for (i=0; i<na; i++)
        polyb[i] = poly_1pz[i];
}

