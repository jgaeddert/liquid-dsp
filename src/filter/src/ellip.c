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
// Elliptic filter design
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "liquid.internal.h"

#define LIQUID_DEBUG_ELLIP_PRINT   0

// forward declarations

// Landen transformation (_n iterations)
void landenf(float _k,
             unsigned int _n,
             float * _v);

// compute elliptic integral K(k) for _n recursions
void ellipkf(float _k,
             unsigned int _n,
             float * _K,
             float * _Kp);

// elliptic degree
float ellipdegf(float _N,
                float _k1,
                unsigned int _n);

// elliptic cd() function (_n recursions)
float complex ellip_cdf(float complex _u,
                        float complex _k,
                        unsigned int _n);

// elliptic inverse cd() function (_n recursions)
float complex ellip_acdf(float complex _u,
                         float complex _k,
                         unsigned int _n);


// elliptic sn() function (_n recursions)
float complex ellip_snf(float complex _u,
                        float complex _k,
                        unsigned int _n);

// elliptic inverse sn() function (_n recursions)
float complex ellip_asnf(float complex _u,
                         float complex _k,
                         unsigned int _n);

// ***************************************************************

// Landen transformation
void landenf(float _k,
             unsigned int _n,
             float * _v)
{
    unsigned int i;
    float k = _k;
    float kp;
    for (i=0; i<_n; i++) {
        kp = sqrtf(1 - k*k);
        k  = (1 - kp)/(1 + kp);
        _v[i] = k;
    }
}

// compute elliptic integral K(k) for _n recursions
void ellipkf(float _k,
             unsigned int _n,
             float * _K,
             float * _Kp)
{
    float kmin = 1e-6f;
    float kmax = sqrtf(1-kmin*kmin);
    
    float K;
    float Kp;

    float kp = sqrtf(1-_k*_k);

    if (_k > kmax) {
        float L = -logf(0.25f*kp);
        K = L + 0.25f*(L-1)*kp*kp;
    } else {
        float v[_n];
        landenf(_k,_n,v);
        K = M_PI * 0.5f;
        unsigned int i;
        for (i=0; i<_n; i++)
            K *= (1 + v[i]);
    }

    if (_k < kmin) {
        float L = -logf(_k*0.25f);
        Kp = L + 0.25f*(L-1)*_k*_k;
    } else {
        float vp[_n];
        landenf(kp,_n,vp);
        Kp = M_PI * 0.5f;
        unsigned int i;
        for (i=0; i<_n; i++)
            Kp *= (1 + vp[i]);
    }

    *_K  = K;
    *_Kp = Kp;
}

// elliptic degree
float ellipdegf(float _N,
                float _k1,
                unsigned int _n)
{
    // compute K1, K1p from _k1
    float K1, K1p;
    ellipkf(_k1,_n,&K1,&K1p);

    // compute q1 from K1, K1p
    float q1 = expf(-M_PI*K1p/K1);

    // compute q from q1
    float q = powf(q1,1.0f/_N);

    // expand numerator, denominator
    unsigned int m;
    float b = 0.0f;
    for (m=0; m<_n; m++)
        b += powf(q,(float)(m*(m+1)));
    float a = 0.0f;
    for (m=1; m<_n; m++)
        a += powf(q,(float)(m*m));

    float g = b / (1.0f + 2.0f*a);
    float k = 4.0f*sqrtf(q)*g*g;
    return k;
}

// elliptic cd() function (_n recursions)
float complex ellip_cdf(float complex _u,
                        float complex _k,
                        unsigned int _n)
{
    float complex wn = ccosf(_u*M_PI*0.5f);
    float v[_n];
    landenf(_k,_n,v);
    unsigned int i;
    for (i=_n; i>0; i--) {
        wn = (1 + v[i-1])*wn / (1 + v[i-1]*wn*wn);
    }
    return wn;
}

// elliptic sn() function (_n recursions)
float complex ellip_snf(float complex _u,
                        float complex _k,
                        unsigned int _n)
{
    float complex wn = csinf(_u*M_PI*0.5f);
    float v[_n];
    landenf(_k,_n,v);
    unsigned int i;
    for (i=_n; i>0; i--) {
        wn = (1 + v[i-1])*wn / (1 + v[i-1]*wn*wn);
    }
    return wn;
}

// elliptic inverse cd() function (_n recursions)
float complex ellip_acdf(float complex _w,
                         float complex _k,
                         unsigned int _n)
{
    float v[_n];
    landenf(_k,_n,v);
    float v1;

    float complex w = _w;
    unsigned int i;
    for (i=0; i<_n; i++) {
        v1 = (i==0) ? _k : v[i-1];
        w = w / (1 + csqrtf(1 - w*w*v1*v1)) * 2.0 / (1+v[i]);
        //printf("  w[%3u] = %12.8f + j*%12.8f\n", i, crealf(w), cimagf(w));
    }

    float complex u = cacosf(w) * 2.0 / M_PI;
    //printf("  u = %12.8f + j*%12.8f\n", crealf(u), cimagf(u));

#if 0
    float K, Kp;
    ellipkf(_k, _n, &K, &Kp);
    float R = Kp / K;
#endif
    return u;
}

// elliptic inverse sn() function (_n recursions)
float complex ellip_asnf(float complex _w,
                         float complex _k,
                         unsigned int _n)
{
    return 1.0 - ellip_acdf(_w,_k,_n);
}

void ellip_azpkf(unsigned int _n,
                 float _fc,
                 float _ep,
                 float _es,
                 float complex * _za,
                 float complex * _pa,
                 float complex * _ka)
{
    // filter specifications
    float fp = _fc;     // pass-band cutoff
    float fs = 1.1*fp;  // stop-band cutoff
    float Gp = 0.95f;   // pass-band gain
    float Gs = 0.01f;   // stop-band gain

    unsigned int n=7;   // number of iterations

    float Wp = 2*M_PI*fp;
    float Ws = 2*M_PI*fs;

    // ripples passband, stopband
    float ep = sqrtf(1.0f/(Gp*Gp) - 1.0f);
    float es = sqrtf(1.0f/(Gs*Gs) - 1.0f);
    printf("ep, es      : %12.8f, %12.8f\n", ep, es);

    float k  = Wp/Ws;           // 0.8889f;
    float k1 = ep/es;           // 0.0165f;
    printf("k           : %12.8f\n", k);
    printf("k1          : %12.8f\n", k1);

    float K,  Kp;
    float K1, K1p;
    ellipkf(k, n, &K,  &Kp);    // K  = 2.23533416, Kp  = 1.66463780
    ellipkf(k1,n, &K1, &K1p);   // K1 = 1.57090271, K1p = 5.49361753
    printf("K,  Kp      : %12.8f, %12.8f\n", K,  Kp);
    printf("K1, K1p     : %12.8f, %12.8f\n", K1, K1p);

    float Nexact = (K1p/K1)/(Kp/K); // 4.69604063
    float N = ceilf(Nexact);        // 5
    N = _n;
    printf("N (exact)   : %12.8f\n", Nexact);
    printf("N           : %12.8f\n", N);

    k = ellipdegf(N,k1,n);      // 0.91427171
    printf("k           : %12.8f\n", k);

    float fs_new = fp/k;        // 4.37506723
    printf("fs_new      : %12.8f\n", fs_new);

    unsigned int L = (unsigned int)(floorf(N/2.0f)); // 2
    unsigned int r = ((unsigned int)N) % 2;
    float u[L];
    unsigned int i;
    for (i=0; i<L; i++) {
        float t = (float)i + 1.0f;
        u[i] = (2.0f*t - 1.0f)/N;
        printf("u[%3u]      : %12.8f\n", i, u[i]);
    }
    float complex zeta[L];
    for (i=0; i<L; i++) {
        zeta[i] = ellip_cdf(u[i],k,n);
        printf("zeta[%3u]   : %12.8f + j*%12.8f\n", i, crealf(zeta[i]), cimagf(zeta[i]));
    }

    // compute filter zeros
    float complex za[L];
    for (i=0; i<L; i++) {
        za[i] = _Complex_I * Wp / (k*zeta[i]);
        printf("za[%3u]     : %12.8f + j*%12.8f\n", i, crealf(za[i]), cimagf(za[i]));
    }

    float complex v0 = -_Complex_I*ellip_asnf(_Complex_I/ep, k1, n)/N;
    printf("v0          : %12.8f + j*%12.8f\n", crealf(v0), cimagf(v0));

    float complex pa[L];
    for (i=0; i<L; i++) {
        pa[i] = Wp*_Complex_I*ellip_cdf(u[i]-_Complex_I*v0, k, n);
        printf("pa[%3u]     : %12.8f + j*%12.8f\n", i, crealf(pa[i]), cimagf(pa[i]));
    }
    float complex pa0 = Wp * _Complex_I*ellip_snf(_Complex_I*v0, k, n);
    printf("pa0         : %12.8f + j*%12.8f\n", crealf(pa0), cimagf(pa0));

    // compute poles
    unsigned int t=0;
    for (i=0; i<L; i++) {
        _pa[t++] =       pa[i];
        _pa[t++] = conjf(pa[i]);
    }
    if (r) _pa[t++] = pa0;
    assert(t==_n);

    t=0;
    for (i=0; i<L; i++) {
        _za[t++] =       za[i];
        _za[t++] = conjf(za[i]);
    }
    assert(t==2*L);

    *_ka = (r==0) ? Gp : 1.0f;
}


