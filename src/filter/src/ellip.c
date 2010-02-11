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

