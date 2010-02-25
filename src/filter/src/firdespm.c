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
// fir (finite impulse response) filter design using Parks-McClellan
// algorithm
//
// References:
//  [McClellan:1973] J. H. McClellan, T. W. Parks, L. R. Rabiner, "A
//      Computer Program for Designing Optimum FIR Linear Phase
//      Digital Filters," IEEE Transactions on Audio and
//      Electroacoustics, vol. AU-21, No. 6, December 1973
//  [Rabiner:197x] L. R. Rabiner, J. H. McClellan, T. W. Parks, "FIR
//      Digital filter Design Techniques Using Weighted Chebyshev
//      Approximations," Proceedings of the IEEE, March 197x.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "liquid.internal.h"

#define LIQUID_FIRDESPM_DEBUG_PRINT 0

// prototypes

//  _n      :   filter semi-length (N = 2*n+1)
//  _fp     :   pass-band edge
//  _fs     :   stop-band edge
//  _K      :   weighting factor
//  _h      :   resulting coefficients
void firdespm(unsigned int _n,
              float _fp,
              float _fs,
              float _K,
              float * _h);

// weighting function
// TODO : allow multiple bands, non-flat weighting function
float firdespm_weight(float _f,
                      float _fp,
                      float _fs,
                      float _K);

//  _N      :   filter length
//  _fp     :   pass-band edge
//  _fs     :   stop-band edge
//  _K      :   weighting factor
//  _h      :   resulting coefficients
void firdespm(unsigned int _N,
              float _fp,
              float _fs,
              float _K,
              float * _h)
{
    // validate input
    if (_fp < 0 || _fp >= 0.5 || _fs <= 0 || _fs > 0.5) {
        fprintf(stderr,"error: firdespm(), invalid bounds\n");
        exit(1);
    } else if (_fp >= _fs) {
        fprintf(stderr,"error: firdespm(), _fp >= _fs\n");
        exit(1);
    } else if (_K < 0) {
        fprintf(stderr,"error: firdespm(), K must be greater than 0\n");
        exit(1);
    } else if (_N==0) {
        fprintf(stderr,"error: firdespm(), filter length (_N) must be greater than 0\n");
        exit(1);
    }

    unsigned int i;

    // TODO : number of approximating functions is dependent upon filter type and symmetry
    // determine number of approximating functions, extremal frequencies
    //  N   :   filter length
    //  n   :   filter semi-length
    //          n = (N-1)/2     N odd
    //          n = N/2         N even
    //  r   :   number of approximating functions
    //          r = n + 1       case 1 : N odd, symmetric impulse
    //          r = n           else
    //  ne  :   number of extremal frequencies
    //  ne_max  maximum number of possible extremal frequencies
    //  [McClellan:1973] Eq. (3), (4), (5), (6)
    //  [Rabiner:197x] Tables II, III, and Eq. (19)
    unsigned int num_bands = 2;     // number of disjoint frequency bands
    unsigned int s = _N % 2;        // s = N odd ? 1 : 0
    unsigned int n = (_N-s)/2;      // filter semi-length
    unsigned int r = n + s;         // number of approximating functions
    unsigned int ne = r + 1;        // number of extrema
    unsigned int ne_max = ne + 2*(num_bands-1);    // maximum number of extrema
                                    // TODO : this will change based on the number of discrete bands
    printf(" N : %u, n : %u, r : %u, ne : %u, ne(max) : %u\n", _N, n, r, ne, ne_max);

    unsigned int d = 20*n; // grid density
    float F[ne_max];    // extremal frequencies

    float D[ne];        // desired response at extremal frequencies
    float H[ne];        // actual response
    float W[ne];        // weighting at extremal frequencies

    int sigma[ne];      // sign of error at extremal frequencies

    float x[ne];        // Chebyshev points on F : cos(2*pi*F[i])
    float c[ne];        // interpolated extremal values (alternating +/- rho)
    float alpha[ne];    // Lagrange interpolation coefficients
    float beta[ne-1];

    // extremal error
    float rho = 0.0f;

    // number of extremal frequencies in the pass-band
    unsigned int np = (unsigned int)( ne*(_fp / (_fp + _fs)));
    if (np < 2)
        np = 2;
    else if (np == ne)
        np = r-1;

    // number of extremal frequencies in the stop-band
    unsigned int ns = ne - np;
    assert(np+ns == ne);

    printf("  np    :   %u\n", np);
    printf("  ns    :   %u\n", ns);

    // initial guess of F
    for (i=0; i<np; i++)
        F[i] = 0.0f + _fp * (float)(i) / (float)(np-1);

    for (i=0; i<ns; i++)
        F[np+i] = _fs  + (0.5-_fs) * (float)(i) / (float)(ns-1);

    // iterate over Remez exchange algorithm
    unsigned int p;
    for (p=0; p<1; p++) {

    // evaluate D (desired response)
    for (i=0; i<ne; i++)
        D[i] = F[i] <= _fp ? 1.0f : 0.0f;

    // evaluate W (weighting factor)
    for (i=0; i<ne; i++)
        W[i] = firdespm_weight(F[i],_fp,_fs,_K);

    // compute Chebyshev points on f : cos(2*pi*f)
    for (i=0; i<ne; i++)
        x[i] = cosf(2*M_PI*F[i]);

    // evaluate alpha[i]
    fpolyfit_lagrange_barycentric(x,ne,alpha);

    // compute rho
    float t0 = 0.0f;
    float t1 = 0.0f;
    for (i=0; i<ne; i++) {
        t0 += alpha[i]*D[i];
        t1 += alpha[i]/W[i] * (i % 2 ? -1.0f : 1.0f);
    }
    rho = t0/t1;
    printf("  rho   :   %12.8f\n", rho);

    // compute polynomial values
    for (i=0; i<ne-1; i++) {
        float pm = i % 2 ? -1 : 1;
        c[i] = D[i] - pm*rho / W[i];

        printf("  c[%3u]    :   %16.8e;\n", i, c[i]);
    }

    // evaluate beta
    // TODO : improve computation of beta knowing alpha
    fpolyfit_lagrange_barycentric(x,ne-1,beta);

    // evaluate the polynomial on the dense set
    FILE * fid = fopen("firdespm_internal_debug.m", "w");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");

    // evaluate trial set of extremal frequencies
    for (i=0; i<ne; i++) {
        H[i] = fpolyval_lagrange_barycentric(x,c,beta,x[i],ne-1);
        sigma[i] = (H[i] - D[i]) > 0 ? 1 : -1;

        fprintf(fid,"fk(%3u) = %16.8e;\n", i+1, F[i]);
        fprintf(fid,"Hk(%3u) = %16.8e;\n", i+1, H[i]);
    }

    // print
    for (i=0; i<ne; i++) {
        printf(" %3u : F=%12.8f, D=%3.1f, W=%12.8f, H=%12.8f, sig=%2d\n",
                  i, F[i], D[i], W[i], H[i], sigma[i]);
    }
    for (i=0; i<ne; i++)
        printf(" alpha[%3u] = %12.4e\n", i, alpha[i]);

    // plotting : evaluate the response
    for (i=0; i<d; i++) {
        float f = 0.5* (float)i / (float)(d-1);
        float xf = cosf(2*M_PI*f);
        float t = fpolyval_lagrange_barycentric(x,c,beta,xf,ne-1);

        fprintf(fid,"f(%3u) = %16.8e; H(%3u) = %16.8e;\n", i+1, f, i+1, t);
    }

    // search for new extremal frequencies
    for (i=0; i<ne; i++) {
        // TODO : replace old algorithm below with more sophisticated approach
    }

    unsigned int m=0;
    float t_prime = 0;
    int dir = 0;

#if 0
    float w_hat;
    float d_hat;
    float e_hat;
#endif
    for (i=0; i<d; i++) {
        // compute candidate frequency
        float f = 0.5* (float)i / (float)(d-1);

        // ignore transition bands
        if (f > _fp && f < _fs)
            continue;

        float xf = cosf(2*M_PI*f);
        float t = fpolyval_lagrange_barycentric(x,c,beta,xf,ne-1);
        //t = fpolyval_lagrange_barycentric(x,c,a,xf,r);

#if 0
        // compute error
        w_hat = firdespm_weight(f,_fp,_fs,_K);
        d_hat = (f < _fp) ? 1.0f : 0.0f;
        e_hat = w_hat*(t - d_hat);
#endif

        // is extremal frequency?
        if ( i == 0 ) {
            t_prime = t;
        } else if (i == 1) {
            dir = (t > t_prime) ? 1 : 0;
        } else if ( (dir && t < t_prime) || (!dir && t > t_prime)) {
            if (m==r)
                continue;

            //fprintf(fid,"fext(%3u) = %16.8e; Hext(%3u) = %16.8e;\n", m+1, f, m+1, t);
            F[m] = f;
            m++;

            dir = 1-dir;
        }

        t_prime = t;
    }
    F[m++] = 0.0f;
    F[m++] = _fp;
    F[m++] = _fs;
    F[m++] = 0.5f;

    // TODO : assert !(m > ne_max)
    // TODO : retain only ne largest extrema

    // sort values(?)
    for (i=0; i<m; i++) {
        unsigned int j;
        for (j=0; j<i; j++) {
            if (F[i] < F[j]) {
                float tmp = F[i];
                F[i] = F[j];
                F[j] = tmp;
            }
        }
    }

    // plotting purposes only
    printf(" m : %u\n", m);
    for (i=0; i<ne; i++) {
        printf("F[%3u] = %12.8f\n", i, F[i]);
        float f = F[i];
        float xf = cosf(2*M_PI*f);
        float t = fpolyval_lagrange_barycentric(x,c,beta,xf,ne-1);
        fprintf(fid,"fext(%3u) = %16.8e; Hext(%3u) = %16.8e;\n", i+1, F[i], i+1, t);
    }

    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,H,'-', fk,Hk,'s',fext,Hext,'x'); grid on;\n");
    fclose(fid);
    printf("internal results written to firdespm_internal_debug.m\n");
    } // p
}

float firdespm_weight(float _f,
                      float _fp,
                      float _fs,
                      float _K)
{
    if (_f <= _fp) {
        return 1.0f;
    } else if (_f >= _fs) {
        return 1.0f / _K;
    } else {
        fprintf(stderr,"warning: firdespm_weight(), _f is not in [_fp,_fs]\n");
    }
    return 1.0f;
}

// iterate over the Remez exchange algorithm
//  _n      :   number of extremal frequencies
//  _fp     :   pass-band frequency
//  _fs     :   stop-band frequency
//  _fext   :   trial set of extremal frequencies
//  _a      :   Lagrange interpolating polynomial coefficients (barycentric)
void firdes_remez(unsigned int _n,
                  float _fp,
                  float _fs,
                  float * _fext,
                  float * _a)
{
    // validate input : assert extremal frequency are in range
    unsigned int i;
    for (i=0; i<_n; i++) {
        if (_fext[i] > _fp && _fext[i] < _fs) {
            fprintf(stderr,"error: firdes_remez(), extremal frequency out of range\n");
            exit(1);
        }
    }

    //
}

// Extremal frequency search (Remez algorithm)
void firdes_remez_fextsearch(unsigned int _n,
                             float _fp,
                             float _fs)
{
}

