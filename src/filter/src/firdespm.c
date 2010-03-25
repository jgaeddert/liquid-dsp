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

#if 0
// structured data type
typedef struct {
    float * F;
    float * D;
    float * W;
    float * H;
    float * E;

    unsigned int grid_size;
    unsigned int grid_density;
    unsigned int r;
} firdespm_s;

typedef firdespm_s * firdespm;

firdespm firdespm_create(unsigned int _h_len);
void firdespm_destroy(firdespm _q);
void firdespm_print(firdespm _q);

void firdespm_execute(firdespm _q, float * _h);
#endif

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

void firdespm_compute_error(unsigned int _r,
                            float * _alpha,
                            float * _x,
                            float * _c,
                            unsigned int _grid_size,
                            float * _F,
                            float * _D,
                            float * _W,
                            float * _E);

// search error curve for _r+1 extremal indices
void firdespm_iext_search(unsigned int _r,
                          unsigned int * _iext,
                          float * _E,
                          unsigned int _grid_size);

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
    unsigned int max_iterations = 7;

    unsigned int grid_size = 0;
    // TODO : compute grid size here
    //for (i=0; i<num_bands; i++)
    //    grid_size += 
    grid_size = 1024;
    float F[grid_size];     // frequencies on the disjoint bounded set
    float D[grid_size];     // desired response
    float W[grid_size];     // weighting

    //float H[grid_size];     // actual filter response
    float E[grid_size];     // error
    
    unsigned int iext[r+1]; // index of extremal frequencies
    float x[r+1];           // Chebyshev points on F[iext[]] : cos(2*pi*f)
    float alpha[r+1];       // Lagrange interpolation coefficients
    float c[r+1];           // interpolated extremal values (alternating +/- rho on E)

    firdespm_init_grid(r,grid_density,num_bands,bands,des,weight,&grid_size,F,D,W);
    FILE * fid = fopen("firdespm_grid.m","w");
    for (i=0; i<grid_size; i++) {
        fprintf(fid,"F(%4u) = %12.4e;\n", i+1, F[i]);
        fprintf(fid,"D(%4u) = %12.4e;\n", i+1, D[i]);
        fprintf(fid,"W(%4u) = %12.4e;\n", i+1, W[i]);
    }
    fclose(fid);

    // initial guess of extremal frequencies evenly spaced on F
    for (i=0; i<=r; i++) {
        iext[i] = (i * (grid_size-1)) / r;
        //printf("iext(%3u) = %u\n", i, iext[i]);
    }

    // TODO : fix grid, weights according to filter type

    // iterate over the Remez exchange algorithm
    unsigned int p;
    for (p=0; p<max_iterations; p++) {
        // compute Chebyshev points on F[iext[]] : cos(2*pi*f)
        for (i=0; i<=r; i++) {
            x[i] = cosf(2*M_PI*F[iext[i]]);
            printf("x[%3u] = %12.8f\n", i, x[i]);
        }
        printf("\n");

        // compute Lagrange interpolating polynomial
        fpolyfit_lagrange_barycentric(x,r+1,alpha);
        for (i=0; i<=r; i++)
            printf("a[%3u] = %12.8f\n", i, alpha[i]);

        // compute rho
        float t0 = 0.0f;    // numerator
        float t1 = 0.0f;    // denominator
        float rho;
        for (i=0; i<r+1; i++) {
            t0 += alpha[i] * D[iext[i]];
            t1 += alpha[i] / W[iext[i]] * (i % 2 ? -1.0f : 1.0f);
        }
        rho = t0/t1;
        printf("  rho   :   %12.4e\n", rho);
        printf("\n");

        // compute polynomial values (interpolants)
        for (i=0; i<=r; i++) {
            c[i] = D[iext[i]] - (i % 2 ? -1 : 1) * rho / W[i];
            printf("c[%3u] = %16.8e\n", i, c[i]);
        }

        // calculate error
        firdespm_compute_error(r,alpha,x,c,grid_size,F,D,W,E);
        FILE * fid = fopen("error.dat","w");
        for (i=0; i<grid_size; i++)
            fprintf(fid,"%16.8e;\n", E[i]);
        fclose(fid);

        // search for new extremal frequencies
        firdespm_iext_search(r, iext, E, grid_size);

        // TODO : check exit criteria
    }

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
        // force endpoint to be upper edge of frequency band
        _F[n-1] = f1;   // according to Janovetz
    }
    *_gridsize = n;

    // TODO : take care of special symmetry conditions here
}

void firdespm_compute_error(unsigned int _r,
                            float * _alpha,
                            float * _x,
                            float * _c,
                            unsigned int _grid_size,
                            float * _F,
                            float * _D,
                            float * _W,
                            float * _E)
{
    unsigned int i;

    float xf;
    float H;
    for (i=0; i<_grid_size; i++) {
        // compute actual response
        xf = cosf(2*M_PI*_F[i]);
        H = fpolyval_lagrange_barycentric(_x,_c,_alpha,xf,_r+1);

        // compute error
        _E[i] = _W[i] * (_D[i] - H);
    }
}

// search error curve for _r+1 extremal indices
void firdespm_iext_search(unsigned int _r,
                          unsigned int * _iext,
                          float * _E,
                          unsigned int _grid_size)
{
    unsigned int i;

    // found extremal frequency indices
    unsigned int found_iext[2*_r];
    unsigned int num_found=0;

    // check for extremum at f=0
    if ( fabsf(_E[0]) > fabsf(_E[1]) )
        found_iext[num_found++] = 0;

    // search inside grid
    for (i=1; i<_grid_size-1; i++) {
        if ( ((_E[i]>0.0) && (_E[i-1]<_E[i]) && (_E[i+1]<_E[i]) ) ||
             ((_E[i]<0.0) && (_E[i-1]>_E[i]) && (_E[i+1]>_E[i]) ) )
        {
            found_iext[num_found++] = i;
        }
    }

    // check for extremum at f=0.5
    if ( fabsf(_E[i]) > fabsf(_E[i-1]) )
        found_iext[num_found++] = i;

#if 1
    for (i=0; i<num_found; i++)
        printf("found_iext[%3u] = %5u : %12.4e\n", i, found_iext[i], _E[found_iext[i]]);
    FILE * fid = fopen("iext.dat","w");
    for (i=0; i<num_found; i++)
        fprintf(fid,"%u;\n", found_iext[i]+1);
    fclose(fid);
#endif

    // search extrema and eliminate smallest
    unsigned int imin=0;    // index of found_iext where _E is a minimum extreme
    unsigned int sign=0;    // sign of error
    unsigned int num_extra = num_found - _r - 1; // number of extra extremal frequencies
    unsigned int alternating_sign;

    while (num_extra) {
        // evaluate sign of first extrema
        sign = _E[found_iext[0]] > 0.0;

        //
        alternating_sign = 1;
        for (i=1; i<num_found; i++) {
            // update new minimum extreme
            if ( fabsf(_E[found_iext[i]]) < fabsf(_E[found_iext[imin]]) )
                imin = i;
    
            if ( sign && _E[found_iext[i]] < 0.0 ) {
                sign = 0;
            } else if ( sign && _E[found_iext[i]] > 0.0 ) {
                sign = 1;
            } else {
                // found two extrema with non-alternating sign
                alternating_sign = 0;
                break;
            }
        }

        // 
        if ( alternating_sign && num_extra==1) {
            //imin = (fabsf(_E[found_iext[0]]) > fabsf(_E[found_iext[num_extra-1]])) ? 0 : num_extra-1;
            if (fabsf(_E[found_iext[0]]) > fabsf(_E[found_iext[num_extra-1]]))
                imin = 0;
            else
                imin = num_extra-1;
        }

        // Delete value in 'found_iext' at 'index imin'.  This
        // is equivalent to shifing all values left one position
        // starting at index imin+1
        printf("deleting found_iext[%3u] = %3u\n", imin, found_iext[imin]);
        memmove( &found_iext[imin],
                 &found_iext[imin+1],
                 (num_found-imin)*sizeof(unsigned int));
        // equivalent code:
        //for (i=imin; i<num_found; i++)
        //    found_iext[i] = found_iext[i+1];

        num_extra--;
        num_found--;
    }

#if 1
    for (i=0; i<num_found; i++)
        printf("found_iext_new[%3u] = %u\n", i, found_iext[i]);
    fid = fopen("iext_new.dat","w");
    for (i=0; i<_r+1; i++)
        fprintf(fid,"%u;\n", found_iext[i]+1);
    fclose(fid);
#endif

    // copy new values
    memmove(_iext, found_iext, (_r+1)*sizeof(unsigned int));

}


