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
// References
//  [Constantinides:1967] A. G. Constantinides, "Frequency
//      Transformations for Digital Filters." IEEE Electronic
//      Letters, vol. 3, no. 11, pp 487-489, 1967.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "liquid.internal.h"

#define LIQUID_IIRDES_DEBUG_PRINT 0

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


// 
// new IIR design
//

// Compute frequency pre-warping factor.  See [Constantinides:1967]
float iirdes_freqprewarp(liquid_iirdes_bandtype _btype,
                         float _fc,
                         float _f0)
{
    float m = 0.0f;
    if (_btype == LIQUID_IIRDES_LOWPASS) {
        // low pass
        m = tanf(M_PI * _fc);
    } else if (_btype == LIQUID_IIRDES_HIGHPASS) {
        // high pass
        m = -cosf(M_PI * _fc) / sinf(M_PI * _fc);
    } else if (_btype == LIQUID_IIRDES_BANDPASS) {
        // band pass
        m = (cosf(2*M_PI*_fc) - cosf(2*M_PI*_f0) )/ sinf(2*M_PI*_fc);
    } else if (_btype == LIQUID_IIRDES_BANDSTOP) {
        // band stop
        m = sinf(2*M_PI*_fc)/(cosf(2*M_PI*_fc) - cosf(2*M_PI*_f0));
    }
    m = fabsf(m);

    return m;
}

// convert to the form:
//          (z^-1 - zd[0])(z^-1 - zd[1]) ... (z^-1 - zd[n-1])
//  H(z) = ---------------------------------------------------
//          (z^-1 - pd[0])(z^-1 - pd[1]) ... (z^-1 - pd[n-1])
void bilinear_zpkf(float complex * _za,
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
        // compute digital zeros (pad with -1s)
        if (i < _nza) {
            float complex zm = _za[i] * _m;
            _zd[i] = (1 + zm)/(1 - zm);
        } else {
            _zd[i] = -1;
        }

        // compute digital poles
        float complex pm = _pa[i] * _m;
        _pd[i] = (1 + pm)/(1 - pm);

        // compute digital gain
        G *= (1 - _pd[i])/(1 - _zd[i]);
    }

#if LIQUID_IIRDES_DEBUG_PRINT
    // print poles and zeros
    printf("zpk_a2df() poles (discrete):\n");
    for (i=0; i<n; i++)
        printf("  pd[%3u] = %12.8f + j*%12.8f\n", i, crealf(_pd[i]), cimagf(_pd[i]));
    printf("zpk_a2df() zeros (discrete):\n");
    for (i=0; i<n; i++)
        printf("  zd[%3u] = %12.8f + j*%12.8f\n", i, crealf(_zd[i]), cimagf(_zd[i]));
    printf("zpk_a2df() gain (discrete):\n");
    printf("  kd      = %12.8f + j*%12.8f\n", crealf(G), cimagf(G));
#endif

    *_kd = G;
}

// convert discrete z/p/k form to transfer function
void iirdes_dzpk2tff(float complex * _zd,
                     float complex * _pd,
                     unsigned int _n,
                     float complex _k,
                     float * _b,
                     float * _a)
{
    unsigned int i;
    float complex q[_n+1];

    // negate poles
    float complex pdm[_n];
    for (i=0; i<_n; i++)
        pdm[i] = -_pd[i];

    // expand poles
    cfpoly_expandroots(pdm,_n,q);
    for (i=0; i<=_n; i++)
        _a[i] = crealf(q[_n-i]);

    // negate zeros
    float complex zdm[_n];
    for (i=0; i<_n; i++)
        zdm[i] = -_zd[i];

    // expand zeros
    cfpoly_expandroots(zdm,_n,q);
    for (i=0; i<=_n; i++)
        _b[i] = crealf(q[_n-i]*_k);
}

// converts discrete-time zero/pole/gain (zpk) recursive (iir)
// filter representation to second-order sections (sos) form
//
//  _zd     :   discrete zeros array (size _n)
//  _pd     :   discrete poles array (size _n)
//  _n      :   number of poles, zeros
//  _kd     :   gain
//
//  _B      :   output numerator matrix (size (L+r) x 3)
//  _A      :   output denominator matrix (size (L+r) x 3)
//
//  L is the number of sections in the cascade:
//      r = _n % 2
//      L = (_n - r) / 2;
void iirdes_dzpk2sosf(float complex * _zd,
                      float complex * _pd,
                      unsigned int _n,
                      float complex _kd,
                      float * _B,
                      float * _A)
{
    int i;
    float tol=1e-6f;

    // find/group complex conjugate pairs (poles)
    float complex zp[_n];
    liquid_cplxpair(_zd,_n,tol,zp);

    // find/group complex conjugate pairs (zeros)
    float complex pp[_n];
    liquid_cplxpair(_pd,_n,tol,pp);

    // TODO : group pole pairs with zero pairs
#if 0
    bool paired[_n];
    unsigned int t[_n];
    memset(paired,0,sizeof(paired));
#endif
    // _n = 2*m + l
    unsigned int r = _n % 2;        // odd/even order
    unsigned int L = (_n - r)/2;    // filter semi-length

#if LIQUID_IIRDES_DEBUG_PRINT
    printf("  n=%u, r=%u, L=%u\n", _n, r, L);
    printf("poles :\n");
    for (i=0; i<_n; i++)
        printf("  p[%3u] = %12.8f + j*%12.8f\n", i, crealf(_pd[i]), cimagf(_pd[i]));
    printf("zeros :\n");
    for (i=0; i<_n; i++)
        printf("  z[%3u] = %12.8f + j*%12.8f\n", i, crealf(_zd[i]), cimagf(_zd[i]));

    printf("poles (conjugate pairs):\n");
    for (i=0; i<_n; i++)
        printf("  p[%3u] = %12.8f + j*%12.8f\n", i, crealf(pp[i]), cimagf(pp[i]));
    printf("zeros (conjugate pairs):\n");
    for (i=0; i<_n; i++)
        printf("  z[%3u] = %12.8f + j*%12.8f\n", i, crealf(zp[i]), cimagf(zp[i]));
#endif

    float complex z0, z1;
    float complex p0, p1;
    for (i=0; i<L; i++) {
        p0 = -pp[2*i+0];
        p1 = -pp[2*i+1];

        z0 = -zp[2*i+0];
        z1 = -zp[2*i+1];

        // expand complex pole pairs
        _A[3*i+0] = 1.0;
        _A[3*i+1] = crealf(p0+p1);
        _A[3*i+2] = crealf(p0*p1);

        // expand complex zero pairs
        _B[3*i+0] = 1.0;
        _B[3*i+1] = crealf(z0+z1);
        _B[3*i+2] = crealf(z0*z1);
    }

    // add zero/pole pair if order is odd
    if (r) {
        p0 = -pp[_n-1];
        z0 = -zp[_n-1];
        
        _A[3*i+0] =  1.0;
        _A[3*i+1] = -pp[_n-1];
        _A[3*i+2] =  0.0;

        _B[3*i+0] =  1.0;
        _B[3*i+1] = -zp[_n-1];
        _B[3*i+2] =  0.0;
    }

    // adjust gain of first element
    _B[0] *= crealf(_kd);
    _B[1] *= crealf(_kd);
    _B[2] *= crealf(_kd);
}

// digital z/p/k low-pass to band-pass
//  _zd     :   digital zeros (low-pass prototype)
//  _pd     :   digital poles (low-pass prototype)
//  _n      :   low-pass filter order
//  _f0     :   center frequency
//  _zdt    :   digital zeros transformed [length: 2*_n]
//  _pdt    :   digital poles transformed [length: 2*_n]
void iirdes_dzpk_lp2bp(liquid_float_complex * _zd,
                       liquid_float_complex * _pd,
                       unsigned int _n,
                       float _f0,
                       liquid_float_complex * _zdt,
                       liquid_float_complex * _pdt)
{
#if 1
    float c0 = cosf(2*M_PI*_f0);

    //unsigned int r = _n % 2;
    //unsigned int L = (_n-r)/2;

    // TODO : keep output conjugates grouped together
    unsigned int i;
    float complex t0;
    for (i=0; i<_n; i++) {
        t0 = 1 + _zd[i];
        _zdt[2*i+0] = 0.5f*(c0*t0 + csqrtf(c0*c0*t0*t0 - 4*_zd[i]));
        _zdt[2*i+1] = 0.5f*(c0*t0 - csqrtf(c0*c0*t0*t0 - 4*_zd[i]));

        t0 = 1 + _pd[i];
        _pdt[2*i+0] = 0.5f*(c0*t0 + csqrtf(c0*c0*t0*t0 - 4*_pd[i]));
        _pdt[2*i+1] = 0.5f*(c0*t0 - csqrtf(c0*c0*t0*t0 - 4*_pd[i]));
    }
#else
    float _beta = 0.2f;   // low-pass prototype cutoff
    _f0 = 0.25f;

    float alpha = cosf(2*M_PI*_f0);
    float f_lo = _f0 - _beta*0.5f;
    float f_hi = _f0 + _beta*0.5f;
    float k = tanf(M_PI*_beta) / tanf(M_PI*(f_hi - f_lo));

    float g1 = -2*alpha*k/(k+1);
    float g2 = (k-1)/(k+1);

    printf("    fc      :   %12.8f\n", _f0);
    printf("    beta    :   %12.8f\n", _beta);
    printf("    f0      :   %12.8f\n", f_lo);
    printf("    f1      :   %12.8f\n", f_hi);
    printf("    alpha   :   %12.8f\n", alpha);
    printf("    k       :   %12.8f\n", k);
    printf("    gamma1  :   %12.8f\n", g1);
    printf("    gamma2  :   %12.8f\n", g2);

    float s = g2+1;

    float complex t0;
    unsigned int i;
    for (i=0; i<_n; i++) {
        t0 = _zd[i]*(g2-1);
        _zdt[2*i+0] = (g1 + csqrtf(g1*g1 - s*s + t0*t0))/(t0 - s);
        _zdt[2*i+1] = (g1 - csqrtf(g1*g1 - s*s + t0*t0))/(t0 - s);

        t0 = _pd[i]*(g2-1);
        _pdt[2*i+0] = (g1 + csqrtf(g1*g1 - s*s + t0*t0))/(t0 - s);
        _pdt[2*i+1] = (g1 - csqrtf(g1*g1 - s*s + t0*t0))/(t0 - s);
    }

#endif
}

