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

//  _n      :   filter semi-length (N = 2*n+1)
//  _fp     :   pass-band edge
//  _fs     :   stop-band edge
//  _K      :   weighting factor
//  _h      :   resulting coefficients
void firdespm(unsigned int _n,
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
    } else if (_n==0) {
        fprintf(stderr,"error: firdespm(), n must be greater than 0\n");
        exit(1);
    }

    unsigned int i;
    unsigned int d = 20*_n; // grid density

    unsigned int r = _n + 2;    // number of extremal frequencies

    float F[r+1];  // extremal frequency locations

    float D[r];    // desired response
    //float H[r];    // actual response
    float W[r];    // weighting
    float c[r];
    float alpha[r];     // Lagrange interpolation coefficients
    float beta[r-1];
    float x[r];

    // extremal error
    float rho = 0.0f;

    // number of extremal frequencies in the pass-band
    unsigned int np = (unsigned int)( r*(_fp / (_fp + _fs)));
    if (np < 2)
        np = 2;
    else if (np == r)
        np = r-1;

    // number of extremal frequencies in the stop-band
    unsigned int ns = r - np;
    assert(np+ns == r);

    printf("  np    :   %u\n", np);
    printf("  ns    :   %u\n", ns);

    // initial guess of F
    for (i=0; i<np; i++)
        F[i] = 0.0f + _fp * (float)(i) / (float)(np-1);

    for (i=0; i<ns; i++)
        F[np+i] = _fs  + (0.5-_fs) * (float)(i) / (float)(ns-1);

    // iterate over Remez exchange algorithm
    unsigned int p;
    for (p=0; p<8; p++) {

    // evaluate D
    for (i=0; i<r; i++)
        D[i] = F[i] <= _fp ? 1.0f : 0.0f;

    // evaluate W
    for (i=0; i<r; i++)
        W[i] = firdespm_weight(F[i],_fp,_fs,_K);

    for (i=0; i<r; i++)
        x[i] = cosf(2*M_PI*F[i]);

    // evaluate alpha[i]
    fpolyfit_lagrange_barycentric(x,r,alpha);

    // print
    for (i=0; i<r; i++) {
        printf("  %3u   : F=%12.8f, D=%3.1f, W=%12.8f, a=%12.4e\n",
                  i, F[i], D[i], W[i], alpha[i]);
    }

    // compute rho
    float t0 = 0.0f;
    float t1 = 0.0f;
    for (i=0; i<r; i++) {
        t0 += alpha[i]*D[i];
        t1 += alpha[i]/W[i] * (i % 2 ? -1.0f : 1.0f);
    }
    rho = t0/t1;
    printf("  rho   :   %12.8f\n", rho);

    // compute polynomial values
    int t = 1;
    for (i=0; i<r; i++) {
        c[i] = D[i] - t*rho / W[i];
        t = -t;

        printf("  c[%3u]    :   %16.8e;\n", i, c[i]);
    }

    // evaluate beta
    fpolyfit_lagrange_barycentric(x,r-1,beta);

    // evaluate the polynomial on the dense set
    FILE * fid = fopen("firdespm_internal_debug.m", "w");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    for (i=0; i<r; i++) {
        float xf = cosf(2*M_PI*F[i]);
        float t  = fpolyval_lagrange_barycentric(x,c,alpha,xf,r);
        fprintf(fid,"fk(%3u) = %16.8e;\n", i+1, F[i]);
        fprintf(fid,"Hk(%3u) = %16.8e;\n", i+1, t);
    }

    unsigned int m=0;
    float t_prime = 0;
    int dir = 0;

    for (i=0; i<d; i++) {
        float f = 0.5* (float)i / (float)(d-1);
        float xf = cosf(2*M_PI*f);
        float t;
        t = fpolyval_lagrange_barycentric(x,c,beta,xf,r-1);
        //t = fpolyval_lagrange_barycentric(x,c,a,xf,r);

        fprintf(fid,"f(%3u) = %16.8e; H(%3u) = %16.8e;\n", i+1, f, i+1, t);

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
    for (i=0; i<r; i++) {
        printf("F[%3u] = %12.8f\n", i, F[i]);
        float f = F[i];
        float xf = cosf(2*M_PI*f);
        float t = fpolyval_lagrange_barycentric(x,c,beta,xf,r);
        fprintf(fid,"fext(%3u) = %16.8e; Hext(%3u) = %16.8e;\n", i+1, F[i], i+1, t);
    }

    fprintf(fid,"figure;\n");
    fprintf(fid,"plot(f,H,'-', fk,Hk,'s',fext,Hext,'x');\n");
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

