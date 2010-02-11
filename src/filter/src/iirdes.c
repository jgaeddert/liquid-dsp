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
// iir (infinite impulse response) filter design
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "liquid.internal.h"

// sorts _z into complex conjugate pairs within a tolerance
void liquid_cplxpair(float complex * _z,
                     unsigned int _n,
                     float _tol,
                     float complex * _p)
{
    bool paired[_n];
    memset(paired,0,sizeof(paired));

    unsigned int i,j,k=0;
    for (i=0; i<_n; i++) {
        // ignore value if already paired
        if (paired[i] || fabsf(cimagf(_z[i])) < _tol)
            continue;

        for (j=0; j<_n; j++) {
            // ignore value if already paired
            if (j==i || paired[j] || fabsf(cimagf(_z[j])) < _tol)
                continue;

            if ( fabsf(cimagf(_z[i])+cimagf(_z[j])) < _tol ) {
                _p[k++] = _z[i];
                _p[k++] = _z[j];
                paired[i] = true;
                paired[j] = true;
                break;
            }
        }
    }
    assert(k <= _n);

    // sort through remaining unpaired values and ensure
    // they are purely real
    for (i=0; i<_n; i++) {
        if (paired[i])
            continue;

        if (cimagf(_z[i]) > _tol) {
            fprintf(stderr,"warning, liquid_cplxpair(), complex numbers cannot be paired\n");
        } else {
            _p[k++] = _z[i];
            paired[i] = true;
        }
    }
}

// converts discrete-time zero/pole/gain (zpk) recursive (iir)
// filter representation to second-order sections (sos) form
//
//  _z      :   zeros array (size _n)
//  _p      :   poles array (size _n)
//  _n      :   number of poles, zeros
//  _k      :   gain
//
//  _B      :   output numerator matrix (size L x 3)
//  _A      :   output denominator matrix (size L x 3)
//
//  L is the number of sections in the cascade:
//      L = (_n + (_n%2)) / 2;
void iirdes_zpk2sos(float complex * _z,
                    float complex * _p,
                    unsigned int _n,
                    float _k,
                    float * _B,
                    float * _A)
{
    unsigned int i;
    float tol=1e-6f;

    // find/group complex conjugate pairs (poles)
    float complex zp[_n];
    liquid_cplxpair(_z,_n,tol,zp);

    // find/group complex conjugate pairs (zeros)
    float complex pp[_n];
    liquid_cplxpair(_p,_n,tol,pp);

    // TODO : group pole pairs with zero pairs
#if 0
    bool paired[_n];
    unsigned int t[_n];
    memset(paired,0,sizeof(paired));
#endif
    // _n = 2*m + l
    unsigned int l = _n % 2;        // odd/even order
    unsigned int m = (_n - l)/2;
    unsigned int L = m+l;

#if 1
    printf("  n=%u, m=%u, l=%u, L=%u\n", _n, m, l, L);
    printf("poles :\n");
    for (i=0; i<_n; i++)
        printf("  p[%3u] = %12.8f + j*%12.8f\n", i, crealf(_p[i]), cimagf(_p[i]));
    printf("zeros :\n");
    for (i=0; i<_n; i++)
        printf("  z[%3u] = %12.8f + j*%12.8f\n", i, crealf(_z[i]), cimagf(_z[i]));

    printf("poles (conjugate pairs):\n");
    for (i=0; i<_n; i++)
        printf("  p[%3u] = %12.8f + j*%12.8f\n", i, crealf(pp[i]), cimagf(pp[i]));
    printf("zeros (conjugate pairs):\n");
    for (i=0; i<_n; i++)
        printf("  z[%3u] = %12.8f + j*%12.8f\n", i, crealf(zp[i]), cimagf(zp[i]));
#endif

    float complex t0, t1;
    for (i=0; i<m; i++) {
        // expand complex pole pairs
        t0 = pp[2*i+0];
        t1 = pp[2*i+1];
        _A[3*i+0] = 1.0;
        _A[3*i+1] = crealf(t0+t1);
        _A[3*i+2] = crealf(t0*t1);

        // expand complex zero pairs
        t0 = zp[2*i+0];
        t1 = zp[2*i+1];
        _B[3*i+0] = 1.0;
        _B[3*i+1] = crealf(t0+t1);
        _B[3*i+2] = crealf(t0*t1);
    }

    // add zero/pole pair if order is odd
    if (l) {
        _A[3*m+0] = 1.0;
        _A[3*m+1] = pp[_n-1];
        _A[3*m+2] = 0.0;

        _B[3*m+0] = 1.0;
        _B[3*m+1] = zp[_n-1];
        _B[3*m+2] = 0.0;
    }

    _B[0] *= _k;
    _B[1] *= _k;
    _B[2] *= _k;

    // TODO : adjust gain
}

// 
// new IIR design
//

// convert to the form:
//          (z^-1 - zd[0])(z^-1 - zd[1]) ... (z^-1 - zd[n-1])
//  H(z) = ---------------------------------------------------
//          (z^-1 - pd[0])(z^-1 - pd[1]) ... (z^-1 - pd[n-1])
void zpk_a2df(float complex * _za,
              unsigned int _nza,
              float complex * _pa,
              unsigned int _npa,
              float complex _ka,
              float _m,
              float complex * _zd,
              float complex * _pd,
              float complex * _kd)
{
    unsigned int n = _npa;
    unsigned int i;
    float complex G = _ka;
    for (i=0; i<n; i++) {
        float complex zm = _za[i] / _m;
        float complex pm = _pa[i] / _m;
        _pd[i] = (pm + 1)/(pm - 1);
        _zd[i] = (i < _nza) ? (zm + 1)/(zm - 1) : 1;
        G *= (1 + _pd[i])/(1 + _zd[i]);
    }

#if 1
    // print poles and zeros
    printf("zpk_a2df() poles (digital):\n");
    for (i=0; i<n; i++)
        printf("  pd[%3u] = %12.8f + j*%12.8f\n", i, crealf(_pd[i]), cimagf(_pd[i]));
    printf("zpk_a2df() zeros (digital):\n");
    for (i=0; i<n; i++)
        printf("  zd[%3u] = %12.8f + j*%12.8f\n", i, crealf(_zd[i]), cimagf(_zd[i]));
    printf("zpk_a2df() gain (digital):\n");
    printf("  kd      = %12.8f + j*%12.8f\n", crealf(G), cimagf(G));
#endif

    *_kd = G;
}

// convert digital z/p/k form to transfer function
void dzpk2tff(float complex * _zd,
              float complex * _pd,
              unsigned int _n,
              float complex _k,
              float * _b,
              float * _a)
{
    unsigned int i;
    float complex q[_n+1];

    // expand poles
    cfpoly_expandroots(_pd,_n,q);
    for (i=0; i<=_n; i++)
        _a[i] = crealf(q[_n-i]);

    // expand zeros
    cfpoly_expandroots(_zd,_n,q);
    for (i=0; i<=_n; i++)
        _b[i] = crealf(q[_n-i]*_k);
}

// convert digital z/p/k form to second-order sections
void dzpk2sosf(float complex * _zd,
               float complex * _pd,
               unsigned int _n,
               float complex _kd,
               float * _B,
               float * _A)
{
    iirdes_zpk2sos(_zd,_pd,_n,_kd,_B,_A);
}


