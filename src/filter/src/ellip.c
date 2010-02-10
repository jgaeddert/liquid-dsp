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

// Landen transformation (one iteration)
float landenf(float _k);

// compute elliptic integral K(k) for _n recursions
void ellipkf(float _k,
             unsigned int _n,
             float * _K,
             float * _Kp);

// elliptic degree
float ellipdegf(float _N,
                float _k1,
                unsigned int _n);

// 
float complex ellip_pqf(float complex _u,
                       float complex _wM,
                       float _k,
                       unsigned int _M);

// 
float complex ellip_apqf(float complex _u,
                        float complex _w0,
                        float _k,
                        unsigned int _M);

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

// Landen transformation (one iteration)
float landenf(float _k)
{
    float kp = sqrtf(1.0f - _k*_k);
    return (1.0f - kp) / (1.0f + kp);
}

// compute elliptic integral K(k) for _n recursions
void ellipkf(float _k,
             unsigned int _n,
             float * _K,
             float * _Kp)
{
    float kn = _k;
    float knp = sqrtf(1.0f - _k*_k);
    float K  = 0.5f*M_PI;
    float Kp = 0.5f*M_PI;
    unsigned int i;
    for (i=0; i<_n; i++) {
        kn  = landenf(kn);
        knp = landenf(knp);

        K  *= (1.0f + kn);
        Kp *= (1.0f + knp);
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

// generic elliptic recursion
float complex ellip_pqf(float complex _u,
                       float complex _wM,
                       float _k,
                       unsigned int _M)
{
    float complex wn = _wM;
    float complex wn_inv = 1.0f / wn;
    float kn;
    unsigned int i;
    for (i=_M; i>0; i--) {
        printf("  1/w[%3u] = %12.8f + j*%12.8f\n", i, crealf(wn_inv), cimagf(wn_inv));
        unsigned int j;
        kn = _k;
        for (j=0; j<i; j++)
            kn = landenf(kn);

        wn = 1.0f / wn_inv;
        wn_inv = 1.0f / (1.0f + kn) * (wn_inv + kn*wn);
    }

    float complex w0 = 1.0f / wn_inv;
    return w0;
}

// elliptic cd() function (_n recursions)
float complex ellip_cdf(float complex _u,
                        float complex _k,
                        unsigned int _n)
{
    float complex wM = ccosf(_u*M_PI*0.5f);
    float complex w0 = ellip_pqf(_u,wM,_k,_n);
    return w0;
}

// elliptic sn() function (_n recursions)
float complex ellip_snf(float complex _u,
                        float complex _k,
                        unsigned int _n)
{
    float complex wM = csinf(_u*M_PI*0.5f);
    float complex w0 = ellip_pqf(_u,wM,_k,_n);
    return w0;
}

// generic elliptic inverse recursion
float complex ellip_apqf(float complex _u,
                         float complex _w0,
                         float _k,
                         unsigned int _M)
{
    float complex wn = _w0;
    float complex kn;
    unsigned int i;
    for (i=1; i<=_M; i++) {
        printf("  w[%3u] = %12.8f + j*%12.8f\n", i, crealf(wn), cimagf(wn));
        unsigned int j;
        kn = _k;
        for (j=0; j<i; j++)
            kn = landenf(kn);

        wn = 2*wn / ((1.0f+kn)*(1.0f + csqrtf(1.0f - kn*kn*wn*wn)));
    }

    return wn;
}

// elliptic inverse cd() function (_n recursions)
float complex ellip_acdf(float complex _u,
                         float complex _k,
                         unsigned int _n)
{
    float complex w0 = ellip_cdf(_u,_k,_n);
    float complex wM = ellip_apqf(_u,w0,_k,_n);
    return cacosf(wM)*(2.0f/M_PI);
}

// elliptic inverse sn() function (_n recursions)
float complex ellip_asnf(float complex _u,
                         float complex _k,
                         unsigned int _n)
{
    printf("**************\n");
    float complex w0 = ellip_snf(_u,_k,_n);     // sn(_u*K,_k)
    float complex wM = ellip_apqf(_u,w0,_k,_n); // csinf(2*pi*_u)
    printf("ellip_asnf:\n");
    printf("  _u = %12.8f + j*%12.8f\n", crealf(_u), cimagf(_u));
    printf("  w0 = %12.8f + j*%12.8f\n", crealf(w0), cimagf(w0));
    printf("  wM = %12.8f + j*%12.8f\n", crealf(wM), cimagf(wM));
    return casinf(wM)*(2.0f/M_PI);
}

