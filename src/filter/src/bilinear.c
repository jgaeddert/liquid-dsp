/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// 
// binlinear z-transforms
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

#define LIQUID_DEBUG_BILINEAR_PRINT 0

// bilinear z-transform using zeros, poles, gain
//
//            (s-z[0])*(s-z[1])*...*(s-z[nz-1])
// H(s) = k * ---------------------------------
//            (s-p[0])*(s-p[1])*...*(s-z[np-1])
//
// inputs
//  _z      :   array of polynomial zeros (length _nz)
//  _nz     :   number of zeros
//  _p      :   array of polynomial poles (length _np)
//  _np     :   number of poles
//  _k      :   scaling factor
//  _m      :   bilateral warping factor
//
// outputs
//  _b      :   digital numerator (length _nz+1)
//  _a      :   digital denominator (length _np+1)
void bilinear_zpk(float complex * _z,
                  unsigned int _nz,
                  float complex * _p,
                  unsigned int _np,
                  float complex _k,
                  float _m,
                  float complex * _b,
                  float complex * _a)
{
    unsigned int i;

    // expand numerator
    unsigned int nb = _nz+1;
    float complex pb[nb];
    cfpoly_expandroots(_z,_nz,pb);
    if (_nz == 0) pb[0] = 1.;

    // expand denominator
    unsigned int na = _np+1;
    float complex pa[na];
    cfpoly_expandroots(_p,_np,pa);
    if (_np == 0) pa[0] = 1.;

    // scale numerator by _k
    for (i=0; i<nb; i++)
        pb[i] *= _k;

#if LIQUID_DEBUG_BILINEAR_PRINT
    printf("bilinear(zpk), numerator:\n");
    for (i=0; i<nb; i++)
        printf("  b[%3u] = %12.8f + j*%12.8f\n", i, crealf(pb[i]), cimagf(pb[i]));

    printf("bilinear(zpk), denominator:\n");
    for (i=0; i<na; i++)
        printf("  a[%3u] = %12.8f + j*%12.8f\n", i, crealf(pa[i]), cimagf(pa[i]));
#endif

    // compute bilinear z-transform on result
    bilinear_nd(pb,nb-1,
                pa,na-1,
                _m,
                _b, _a);
}

// bilinear z-transform using polynomial expansion in numerator and
// denominator
//
//          b[0] + b[1]*s + ... + b[nb]*s^(nb-1)
// H(s) =   ------------------------------------
//          a[0] + a[1]*s + ... + a[na]*s^(na-1)
//
// computes H(z) = H( s -> _m*(z-1)/(z+1) ) and expands as
//
//          bd[0] + bd[1]*z^-1 + ... + bd[nb]*z^-n
// H(z) =   --------------------------------------
//          ad[0] + ad[1]*z^-1 + ... + ad[nb]*z^-m
//
// inputs
//  _b          :   numerator array (length _b_order+1)
//  _b_order    :   polynomial order of _b
//  _a          :   denominator array (length _a_order+1)
//  _a_order    :   polynomial order of _a
//  _m          :   bilateral warping factor
//
// outputs
//  _bd         :   digital filter numerator (length _b_order+1)
//  _ad         :   digital filter numerator (length _a_order+1)
void bilinear_nd(float complex * _b,
                 unsigned int _b_order,
                 float complex * _a,
                 unsigned int _a_order,
                 float _m,
                 float complex * _bd,
                 float complex * _ad)
{
    if (_b_order > _a_order) {
        fprintf(stderr,"error: bilinear_nd(), numerator order cannot be higher than denominator\n");
        exit(1);
    }

#if LIQUID_DEBUG_BILINEAR_PRINT
    printf("***********************************\n");
    printf("bilinear(nd), numerator order   : %u\n", _b_order);
    printf("bilinear(nd), denominator order : %u\n", _a_order);
#endif

    // ...
    unsigned int nb = _b_order+1;   // input numerator polynomial array length
    unsigned int na = _a_order+1;   // input denominator polynomial array length

    unsigned int i, j;

    // clear output arrays (both are length na = _a_order+1)
    for (i=0; i<na; i++) _bd[i] = 0.;
    for (i=0; i<na; i++) _ad[i] = 0.;

    // temporary polynomial: (1 + 1/z)^(k) * (1 - 1/z)^(n-k)
    int poly_1pz[na];

    float mk=1.0f;

    // multiply denominator by ((1-1/z)/(1+1/z))^na and expand
    for (i=0; i<na; i++) {
        // expand the polynomial (1+x)^i * (1-x)^(_a_order-i)
        poly_binomial_expand_pm(_a_order,
                                _a_order-i,
                                poly_1pz);

#if LIQUID_DEBUG_BILINEAR_PRINT
        printf("  %-4u : a=%12.4e + j*%12.4e, mk=%12.8f\n", i, crealf(_a[i]), cimagf(_a[i]), mk);
        for (j=0; j<na; j++)
            printf("    poly_1pz[%3u] = %6d : %12.4f + j*%12.4f\n", j, poly_1pz[j], crealf(_a[i]*mk*poly_1pz[j]),
                                                                                    cimagf(_a[i]*mk*poly_1pz[j]));
#endif

        // accumulate polynomial coefficients
        for (j=0; j<na; j++)
            _ad[j] += _a[i]*mk*poly_1pz[j];

        // update multiplier
        mk *= _m;
    }

    // multiply numerator by ((1-1/z)/(1+1/z))^na and expand
    mk = 1.0f;
    for (i=0; i<nb; i++) {
        // expand the polynomial (1+x)^i * (1-x)^(_a_order-i)
        poly_binomial_expand_pm(_a_order,
                                _a_order-i,
                                poly_1pz);

        // accumulate polynomial coefficients
        for (j=0; j<na; j++)
            _bd[j] += _b[i]*mk*poly_1pz[j];

        // update multiplier
        mk *= _m;
    }

    // normalize by a[0]
    float complex a0_inv = 1.0f / _ad[0];
    for (i=0; i<na; i++) {
        _bd[i] *= a0_inv;
        _ad[i] *= a0_inv;
    }
}

