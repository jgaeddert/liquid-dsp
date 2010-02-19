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

    unsigned int nt = _n + 2;

    float a[_n+2];  // Lagrange interpolation coefficients
    float F[_n+3];  // extremal frequency locations
    float e[_n+3];  // extremal frequency error values

    float D[nt];    // desired response
    float H[nt];    // actual response
    float W[nt];    // weighting

    // extremal error
    float rho = 1.0f;

    // number of extremal frequencies in the pass-band
    unsigned int np = (unsigned int)( nt*(_fp / (_fp + _fs)));
    if (np < 2)
        np = 2;
    else if (np == nt)
        np = nt-1;

    // number of extremal frequencies in the stop-band
    unsigned int ns = nt - np;
    assert(np+ns == nt);

    printf("  np    :   %u\n", np);
    printf("  ns    :   %u\n", ns);

    // initial guess of F
    for (i=0; i<np; i++)
        F[i] = 0.0f + _fp * (float)(i) / (float)(np-1);

    for (i=0; i<ns; i++)
        F[np+i] = _fs  + (0.5-_fs) * (float)(i) / (float)(ns);

    // evaluate D
    for (i=0; i<nt; i++)
        D[i] = F[i] <= _fp ? 1.0f : 0.0f;

    // evaluate W
    for (i=0; i<nt; i++)
        W[i] = firdespm_weight(F[i],_fp,_fs,_K);

    float x[nt];
    for (i=0; i<nt; i++)
        x[i] = cosf(2*M_PI*F[i]);

    // evaluate a[i]
    for (i=0; i<nt; i++) {
        unsigned int k;
        a[i] = i % 2 ? 1.0f : -1.0f;
        for (k=0; k<nt; k++) {
            if (i==k) continue;

            a[i] *= 1.0f / (x[i] - x[k]);
        }
    }

    // print
    for (i=0; i<nt; i++) {
        printf("  %3u   : F=%12.8f, D=%3.1f, W=%12.8f, a=%12.4e\n",
                  i, F[i], D[i], W[i], a[i]);
    }

    // compute rho
    float t0 = 0.0f;
    float t1 = 0.0f;
    for (i=0; i<nt; i++) {
        t0 += a[i]*D[i];
        t1 += a[i]/W[i] * (i % 2 ? 1.0f : -1.0f);
    }
    rho = t0/t1;
    printf("  rho   :   %12.8f\n", rho);

    // compute polynomial values
    float c[nt-1];
    int t = 1;
    for (i=0; i<nt-1; i++) {
        c[i] = D[i] - t*rho / W[i];
        t = -t;

        printf("  c[%3u]    :   %12.4e;\n", i, c[i]);
    }

    // evaluate alpha
    float alpha[nt-1];
    for (i=0; i<nt-1; i++) {
        unsigned int k;
        alpha[i] = i % 2 ? 1.0f : -1.0f;
        for (k=0; k<nt-1; k++) {
            if (i==k) continue;

            alpha[i] *= 1.0f / (x[i] - x[k]);
        }
    }

    // evaluate the polynomial on the dense set
    FILE * fid = fopen("firdespm_internal_debug.m", "w");
    fprintf(fid,"clear all;\n");
    fprintf(fid,"close all;\n");
    for (i=0; i<d; i++) {
        float f = (float)i / (float)(2*d);
        float xf = cosf(2*M_PI*f);

        unsigned int k;
        t0 = 0.0f;
        t1 = 0.0f;
        for (k=0; k<nt-1; k++) {
            t0 += c[k]*alpha[k]/(xf - x[k]);
            t1 +=      alpha[k]/(xf - x[k]);
        }
        fprintf(fid,"H(%3u) = %12.4e;\n", i+1, t0/t1);
    }
    printf("internal results written to firdespm_internal_debug.m\n");
    fclose(fid);
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

