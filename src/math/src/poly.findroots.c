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
// polynomial methods
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "liquid.internal.h"


// finds the complex roots of the polynomial using the Durand-Kerner method
void POLY(_findroots)(T * _p,
                      unsigned int _k,
                      T * _roots)
{
    if (_k < 2) {
        fprintf(stderr,"%s_findroots(), order must be greater than 0\n", POLY_NAME);
        exit(1);
    } else if (_p[_k-1] != 1) {
        fprintf(stderr,"%s_findroots(), _p[_k-1] must be equal to 1\n", POLY_NAME);
        exit(1);
    }

    unsigned int i;
    unsigned int num_roots = _k-1;
    T r0[num_roots];
    T r1[num_roots];

    // find intial magnitude
    float g     = 0.0f;
    float gmax  = 0.0f;
    for (i=0; i<_k; i++) {
        g = cabsf(_p[i]);
        if (i==0 || g > gmax)
            gmax = g;
    }

    // initialize roots
    T t0 = 0.9f * (1 + gmax) * cexpf(_Complex_I*1.1526f);
    T t  = 1.0f;
    for (i=0; i<num_roots; i++) {
        r0[i] = t;
        t *= t0;
    }

    unsigned int max_num_iterations = 50;
    int continue_iterating = 1;
    unsigned int j, k;
    T f;
    T fp;
    //for (i=0; i<num_iterations; i++) {
    i = 0;
    while (continue_iterating) {
#if 0
        printf("%s_findroots(), i=%3u :\n", POLY_NAME, i);
        for (j=0; j<num_roots; j++)
            printf("  r[%3u] = %12.8f + j*%12.8f\n", j, crealf(r0[j]), cimagf(r0[j]));
#endif
        for (j=0; j<num_roots; j++) {
            f = POLY(val)(_p,_k,r0[j]);
            fp = 1;
            for (k=0; k<num_roots; k++) {
                if (k==j) continue;
                fp *= r0[j] - r0[k];
            }
            r1[j] = r0[j] - f / fp;
        }

        // stop iterating if roots have settled
        float delta=0.0f;
        T e;
        for (j=0; j<num_roots; j++) {
            e = r0[j] - r1[j];
            delta += crealf(e*conjf(e));
        }
        delta /= num_roots * gmax;
#if 0
        printf("delta[%3u] = %12.4e\n", i, delta);
#endif

        if (delta < 1e-6f || i == max_num_iterations)
            continue_iterating = 0;

        memmove(r0, r1, num_roots*sizeof(T));
        i++;
    }

    for (i=0; i<_k; i++)
        _roots[i] = r1[i];
}

// finds the complex roots of the polynomial using Bairstow's method
void POLY(_findroots_bairstow)(T * _p,
                               unsigned int _k,
                               float complex * _roots)
{
}

// iterate over Bairstow's method
void POLY(_findroots_bairstow_recursion)(T * _p,
                                         unsigned int _k,
                                         T * _p1,
                                         T * _u,
                                         T * _v)
{
    // validate length
    if (_k < 4) {
        fprintf(stderr,"findroots_bairstow_recursion(), invalid polynomial length: %u\n", _k);
        exit(1);
    }

    // initial estimates for u, v
    // TODO : ensure no division by zero
    T u = _p[_k-2] / _p[_k-1];
    T v = _p[_k-3] / _p[_k-1];
    
    unsigned int n = _k-1;
    T c,d,g,h;
    T q;
    T du, dv;

    // reduced polynomials
    T b[_k];
    T f[_k];
    b[n] = b[n-1] = 0;
    f[n] = f[n-1] = 0;

    int i;
    unsigned int k=0;
    unsigned int max_num_iterations=50;
    int continue_iterating = 1;

    while (continue_iterating) {
        // update reduced polynomial coefficients
        for (i=n-2; i>=0; i--) {
            b[i] = _p[i+2] - u*b[i+1] - v*b[i+2];
            f[i] =  b[i+2] - u*f[i+1] - v*f[i+2];
        }
        c = _p[1] - u*b[0] - v*b[1];
        g =  b[1] - u*f[0] - v*f[1];
        d = _p[0] - v*b[0];
        h =  b[0] - v*f[0];

        // compute scaling factor
        q  = 1/(v*g*g + h*(h-u*g));

        // compute u, v steps
        du = - q*(-h*c   + g*d);
        dv = - q*(-g*v*c + (g*u-h)*d);

#if 0
        printf("bairstow [%u] :\n", k);
        printf("  u : %12.8f + j*%12.8f\n", crealf(u), cimagf(u));
        printf("  v : %12.8f + j*%12.8f\n", crealf(v), cimagf(v));
        printf("  b : \n");
        for (i=0; i<n-2; i++)
            printf("      %12.8f + j*%12.8f\n", crealf(b[i]), cimagf(b[i]));
        printf("  fb : \n");
        for (i=0; i<n-2; i++)
            printf("      %12.8f + j*%12.8f\n", crealf(f[i]), cimagf(f[i]));
        printf("  c : %12.8f + j*%12.8f\n", crealf(c), cimagf(c));
        printf("  g : %12.8f + j*%12.8f\n", crealf(g), cimagf(g));
        printf("  d : %12.8f + j*%12.8f\n", crealf(d), cimagf(d));
        printf("  h : %12.8f + j*%12.8f\n", crealf(h), cimagf(h));

        printf("  step : %12.8f + j*%12.8f\n", crealf(du+dv), cimagf(du+dv));
#endif

        // adjust u, v
        u += du;
        v += dv;

        // increment iteration counter
        k++;

        // exit conditions
        if (cabsf(du+dv) < 1e-6f || k == max_num_iterations)
            continue_iterating = 0;
    }

    for (i=0; i<_k-2; i++)
        _p1[i] = b[i];

    *_u = u;
    *_v = v;

}
