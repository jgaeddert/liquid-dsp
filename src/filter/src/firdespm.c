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
//      Electroacoustics, vol. AU-21, No. 6, December 1973.
//  [Rabiner:1975] L. R. Rabiner, J. H. McClellan, T. W. Parks, "FIR
//      Digital filter Design Techniques Using Weighted Chebyshev
//      Approximations," Proceedings of the IEEE, March 1975.

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

// initialize the frequency grid on the disjoint bounded set
void firdespm_init_grid(unsigned int _r,            // number of approximating functions
                        unsigned int _grid_density,
                        unsigned int _num_bands,
                        float * _bands,
                        float * _des,
                        float * _weight,
                        unsigned int *_gridsize,
                        float * _F,
                        float * _D,
                        float * _W);


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

#if 0
    float bands[2][2] = {
        {0.0f, _fp},
        {_fs,  0.5f}};
#else
    float bands[4]  = {0.0f, _fp, _fs, 0.5f};
    float des[2]    = {1.0f, 0.0f};
    float weight[2] = {1.0f, 1.0f};
#endif

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

    unsigned int grid_density = 16;
    unsigned int grid_size = 0;
    //for (i=0; i<num_bands; i++)
    //    grid_size += 
    grid_size = 1024;
    float F[grid_size];
    float D[grid_size];
    float W[grid_size];

    firdespm_init_grid(r,grid_density,num_bands,bands,des,weight,&grid_size,F,D,W);
    FILE * fid = fopen("grid.dat","w");
    for (i=0; i<grid_size; i++)
        fprintf(fid,"%12.4e;\n", F[i]);
    fclose(fid);

    // 

#if 0
    // evaluate Lagrange polynomial on evenly spaced points
    unsigned int b=r;
    float G[2*b-1];
    for (i=0; i<=b; i++) {
        float f = (float)(i) / (float)(2*b-1);
        float xf = cosf(2*M_PI*f);
        float cf = fpolyval_lagrange_barycentric(x,c,beta,xf,ne-1);
        G[i] = cf;
        G[2*b-i-1] = cf;
    }
    //for (i=0; i<2*b-1; i++)
    //    printf("G(%3u) = %12.4e;\n", i+1, G[i]);

    // compute inverse DFT
    float h[b];
    for (i=0; i<b; i++) {
        h[i] = 0.0f;
        unsigned int j;
        for (j=0; j<2*b-1; j++) {
            float f = (float)(i) / (float) (2*b-1);
            h[i] += G[j] * cosf(2*M_PI*f*j);
        }
        h[i] /= 2*b-1;
    }
    for (i=0; i<b; i++)
        printf("h(%3u) = %12.8f;\n", i+1, h[i]);

    // TODO : perform transformation here for different filter types
#endif
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

// initialize the frequency grid on the disjoint bounded set
void firdespm_init_grid(unsigned int _r,            // number of approximating functions
                        unsigned int _grid_density,
                        unsigned int _num_bands,
                        float * _bands,
                        float * _des,
                        float * _weight,
                        unsigned int *_gridsize,
                        float * _F,
                        float * _D,
                        float * _W)
{
    unsigned int i,j;

    // validate input

    // frequency step size
    float df = 0.5f/(_grid_density*_r);
    printf("df : %12.8f\n", df);

    // number of grid points counter
    unsigned int n = 0;

    // TODO : take care of special symmetry conditions here

    float f0, f1;
    for (i=0; i<_num_bands; i++) {
        // extract band edges
        f0 = _bands[2*i+0];
        f1 = _bands[2*i+1];
        printf("band : [%12.8f %12.8f]\n",f0,f1);

        // compute the number of gridpoints in this band
        unsigned int num_points = (unsigned int)( (f1-f0)/df + 0.5 );

        // ensure at least one point per band
        if (num_points < 1) num_points = 1;

        // add points to grid
        for (j=0; j<num_points; j++) {
            // add frequency points
            _F[n] = f0 + j*df;

            // compute desired response
            // TODO : use function pointer?
            _D[n] = _des[i];

            // compute weight
            // TODO : use function pointer?
            _W[n] = _weight[i];

            n++;
        }
        _F[n-1] = f1;   // according to Janovetz
    }
    *_gridsize = n;

    // TODO : take care of special symmetry conditions here
}



