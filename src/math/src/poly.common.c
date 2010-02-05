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
// common polynomial expansions
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"


// expands the polynomial:
//  (1+x)^n
// as
//  c[0] + c[1]*x + c[2]*x^2 + ... + c[n]*x^n
void poly_binomial_expand(unsigned int _n,
                          int * _c)
{
    // easy way...
    unsigned int k;
    for (k=0; k<=_n; k++)
        _c[k] = (int) liquid_nchoosek(_n,k);
}


// expands the polynomial:
//  (1+x)^k * (1-x)^(n-k)
// as
//  c[0] + c[1]*x + c[2]*x^2 + ... + c[n]*x^n
//
//  tests:  n=6, k=1 : c[7]=[1  -4   5   0  -5   4  -1]
//          n=5, k=2 : c[6]=[1  -1  -2   2   1  -1]
void poly_binomial_expand_pm(unsigned int _n,
                             unsigned int _k,
                             int * _c)
{
    int cp[_k+1];       // coefficients array for (1+x)^k
    int cm[_n-_k+1];    // coefficients array for (1-x)^(n-k)
    unsigned int i;
    for (i=0; i<_n+1; i++)
        _c[i]  = 0;

    // compute (1+x)^k
    for (i=0; i<=_k; i++) {
        cp[i] = (int) liquid_nchoosek(_k,i);
    }

    // compute (1-x)^(n-k)
    for (i=0; i<=_n-_k; i++) {
        cm[i] = (i%2 ? -1 : 1) * (int) liquid_nchoosek(_n-_k,i);
    }

    // multiply both polynomials
    unsigned int j;
    unsigned int t;
    for (i=0; i<=_k; i++) {
        for (j=0; j<=_n-_k; j++) {
            t = i+j;
            _c[t] += cp[i] * cm[j];
        }
    }
}

