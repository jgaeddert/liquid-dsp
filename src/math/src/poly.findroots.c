/*
 * Copyright (c) 2007 - 2019 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

//
// polynomial methods
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "liquid.internal.h"

// debug polynomial root-finding methods?
#define LIQUID_POLY_FINDROOTS_DEBUG     0

// 
// forward declaration of internal methods
//

// iterate over Bairstow's method, finding quadratic factor x^2 + u*x + v and
// return flag indicating success/failure
int POLY(_findroots_bairstow_recursion)(T *          _p,
                                        unsigned int _k,
                                        T *          _p1,
                                        T *          _u,
                                        T *          _v);

// run multiple iterations of Bairstow's method with different starting
// conditions looking for convergence
int POLY(_findroots_bairstow_persistent)(T *          _p,
                                         unsigned int _k,
                                         T *          _p1,
                                         T *          _u,
                                         T *          _v);

// sort roots in ascending order of real component; complex pairs order
// root with positive imaginary component before root with negative
// imaginary component.
int POLY(_sort_roots_compare)(const void * _a,
                              const void * _b);

// finds the complex roots of the polynomial
//  _p      :   polynomial array, ascending powers [size: _k x 1]
//  _k      :   polynomials length (poly order = _k - 1)
//  _roots  :   resulting complex roots [size: _k-1 x 1]
void POLY(_findroots)(T *          _p,
                      unsigned int _k,
                      TC *         _roots)
{
    // find roots of polynomial using Bairstow's method (more
    // accurate and reliable than Durand-Kerner)
    POLY(_findroots_bairstow)(_p,_k,_roots);

    // sort roots for consistent ordering
    qsort(_roots, _k-1, sizeof(TC), &POLY(_sort_roots_compare));
}

// finds the complex roots of the polynomial using the Durand-Kerner method
//  _p      :   polynomial array, ascending powers [size: _k x 1]
//  _k      :   polynomials length (poly order = _k - 1)
//  _roots  :   resulting complex roots [size: _k-1 x 1]
void POLY(_findroots_durandkerner)(T *          _p,
                                   unsigned int _k,
                                   TC *         _roots)
{
    if (_k < 2) {
        fprintf(stderr,"%s_findroots_durandkerner(), order must be greater than 0\n", POLY_NAME);
        exit(1);
    } else if (_p[_k-1] != 1) {
        fprintf(stderr,"%s_findroots_durandkerner(), _p[_k-1] must be equal to 1\n", POLY_NAME);
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
        g = T_ABS(_p[i]);
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
#if LIQUID_POLY_FINDROOTS_DEBUG
        printf("%s_findroots(), i=%3u :\n", POLY_NAME, i);
        for (j=0; j<num_roots; j++)
            printf("  r[%3u] = %12.8f + j*%12.8f\n", j, crealf(r0[j]), cimagf(r0[j]));
#endif
        for (j=0; j<num_roots; j++) {
            f = POLY(_val)(_p,_k,r0[j]);
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
#if LIQUID_POLY_FINDROOTS_DEBUG
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
//  _p      :   polynomial array, ascending powers [size: _k x 1]
//  _k      :   polynomials length (poly order = _k - 1)
//  _roots  :   resulting complex roots [size: _k-1 x 1]
void POLY(_findroots_bairstow)(T *          _p,
                               unsigned int _k,
                               TC *         _roots)
{
    T p0[_k];       // buffer 0
    T p1[_k];       // buffer 1
    T * p   = NULL; // input polynomial
    T * pr  = NULL; // output (reduced) polynomial

    unsigned int i;
    unsigned int k=0;   // output roots counter
    memmove(p0, _p, _k*sizeof(T));

    T u, v;

    unsigned int n = _k;        // input counter (decrementer)
    unsigned int r = _k % 2;    // polynomial length odd? (order even?)
    unsigned int L = (_k-r)/2;  // semi-length
    for (i=0; i<L-1+r; i++) {
        // set polynomial and reduced polynomial buffer pointers
        p  = (i % 2) == 0 ? p0 : p1;
        pr = (i % 2) == 0 ? p1 : p0;

        // initial estimates for u, v
        if (p[n-1] == 0) {
            fprintf(stderr,"warning: poly_findroots_bairstow(), irreducible polynomial");
            p[n-1] = 1e-12;
        }
        u = p[n-2] / p[n-1];
        v = p[n-3] / p[n-1];

        // compute factor using Bairstow's recursion
        if (n > 3)
            POLY(_findroots_bairstow_persistent)(p,n,pr,&u,&v);

        // compute complex roots of x^2 + u*x + v
        TC r0 = 0.5f*(-u + csqrtf(u*u - 4.0*v));
        TC r1 = 0.5f*(-u - csqrtf(u*u - 4.0*v));
        //printf("roots: r0=%12.8f + j*%12.8f, r1=%12.8f + j*%12.8f\n\n",
        //        crealf(r0), cimagf(r0), crealf(r1), cimagf(r1));

        // append result to output
        _roots[k++] = r0;
        _roots[k++] = r1;

#if LIQUID_POLY_FINDROOTS_DEBUG
        // print debugging info
        unsigned int j;
        printf("initial polynomial:\n");
        for (j=0; j<n; j++)
            printf("  p[%3u]  = %12.8f + j*%12.8f\n", j, crealf(p[j]), cimagf(p[j]));

        printf("polynomial factor: x^2 + u*x + v\n");
        printf("  u : %12.8f + j*%12.8f\n", crealf(u), cimagf(u));
        printf("  v : %12.8f + j*%12.8f\n", crealf(v), cimagf(v));

        printf("roots:\n");
        printf("  r0 : %12.8f + j*%12.8f\n", crealf(r0), cimagf(r0));
        printf("  r1 : %12.8f + j*%12.8f\n", crealf(r1), cimagf(r1));

        printf("reduced polynomial:\n");
        for (j=0; j<n-2; j++)
            printf("  pr[%3u] = %12.8f + j*%12.8f\n", j, crealf(pr[j]), cimagf(pr[j]));
#endif

        // decrement new (reduced) polynomial size by 2
        n -= 2;
    }

    if (r==0) {
#if LIQUID_POLY_FINDROOTS_DEBUG
        assert(n==2);
#endif
        _roots[k++] = -pr[0]/pr[1];
    }
}

// iterate over Bairstow's method, finding quadratic factor x^2 + u*x + v
//  _p      :   polynomial array, ascending powers [size: _k x 1]
//  _k      :   polynomials length (poly order = _k - 1)
//  _p1     :   reduced polynomial (output) [size: _k-2 x 1]
//  _u      :   input: initial estimate for u; output: resulting u
//  _v      :   input: initial estimate for v; output: resulting v
int POLY(_findroots_bairstow_recursion)(T *          _p,
                                        unsigned int _k,
                                        T *          _p1,
                                        T *          _u,
                                        T *          _v)
{
    // validate length
    if (_k < 3) {
        fprintf(stderr,"findroots_bairstow_recursion(), invalid polynomial length: %u\n", _k);
        exit(1);
    }

    // initial estimates for u, v
    T u = *_u;
    T v = *_v;
    //printf("bairstow recursion, u=%12.4e + j*%12.4e, v=%12.4e + j*%12.4e\n",
    //        crealf(u), cimagf(u), crealf(v), cimagf(v));
    
    unsigned int n = _k-1;
    T c,d,g,h;
    T q0, q1, q;
    T du, dv;

    // reduced polynomials
    T b[_k];
    T f[_k];
    b[n] = b[n-1] = 0;
    f[n] = f[n-1] = 0;

    int i;
    unsigned int num_iterations     =   0;  // current iteration count
    unsigned int num_iterations_max =  50;  // maximum iteration count
    int          rc                 =   0;  //
    while (1) {
        // check iteration count
        if (num_iterations == num_iterations_max) {
            // failed to converge
            rc = 1;
            break;
        }
        num_iterations++;

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
        q0 = v*g*g;
        q1 = h*(h-u*g);
        double metric = T_ABS(q0+q1);
        if ( metric < 1e-12 ) {
            q = 0;
            u *= 0.5;
            v *= 0.5;
            continue;
        } else {
            q  = 1/(v*g*g + h*(h-u*g));
        }
        // compute u, v steps
        du = - q*(-h*c   + g*d);
        dv = - q*(-g*v*c + (g*u-h)*d);

        double step = T_ABS(du)+T_ABS(dv);

#if 0
        printf(" %3u : q0=%12.4e, q1=%12.4e, q=%12.4e, metric=%12.4e, u=%8.3f, v=%8.3f, step=%12.4e\n",
                num_iterations, T_ABS(q0), T_ABS(q1), T_ABS(q), metric,
                crealf(u), crealf(v),
                step);
#endif

#if LIQUID_POLY_FINDROOTS_DEBUG
        // print debugging info
        printf("bairstow [%u] :\n", num_iterations);
        printf("  u     : %12.4e + j*%12.4e\n", crealf(u), cimagf(u));
        printf("  v     : %12.4e + j*%12.4e\n", crealf(v), cimagf(v));
        printf("  b     : \n");
        for (i=0; i<n-2; i++)
            printf("      %12.4e + j*%12.4e\n", crealf(b[i]), cimagf(b[i]));
        printf("  fb    : \n");
        for (i=0; i<n-2; i++)
            printf("      %12.4e + j*%12.4e\n", crealf(f[i]), cimagf(f[i]));
        printf("  c     : %12.4e + j*%12.4e\n", crealf(c), cimagf(c));
        printf("  g     : %12.4e + j*%12.4e\n", crealf(g), cimagf(g));
        printf("  d     : %12.4e + j*%12.4e\n", crealf(d), cimagf(d));
        printf("  h     : %12.4e + j*%12.4e\n", crealf(h), cimagf(h));
        printf("  q     : %12.4e + j*%12.4e\n", crealf(q), cimagf(q));
        printf("  du    : %12.4e + j*%12.4e\n", crealf(du), cimagf(du));
        printf("  dv    : %12.4e + j*%12.4e\n", crealf(dv), cimagf(dv));

        printf("  step : %12.4e + j*%12.4e\n", crealf(du+dv), cimagf(du+dv));
#endif
        // adjust u, v by step size
        u += du;
        v += dv;

        // exit conditions
        if (step < 1e-12f)
            break;
    }

    // set resulting reduced polynomial
    for (i=0; i<_k-2; i++)
        _p1[i] = b[i];

    // set output pairs
    *_u = u;
    *_v = v;
    return rc;
}

// run multiple iterations of Bairstow's method with different starting
// conditions looking for convergence
int POLY(_findroots_bairstow_persistent)(T *          _p,
                                         unsigned int _k,
                                         T *          _p1,
                                         T *          _u,
                                         T *          _v)
{
    unsigned int i, num_iterations_max = 10;
    for (i=0; i<num_iterations_max; i++) {
        //printf("#\n# persistence %u\n#\n", i);
        if (POLY(_findroots_bairstow_recursion)(_p, _k, _p1, _u, _v)==0) {
            // success
            return 0;
        } else if (i < num_iterations_max-1) {
            // didn't converge; adjust starting point using consistent and
            // reproduceable starting point
            *_u = cosf((float)i * 1.1f) * expf((float)i * 0.2f);
            *_v = sinf((float)i * 1.1f) * expf((float)i * 0.2f);
        }
    }

    // could not converge
    //printf("# persistence failed to converge, u=%12.8f, v=%12.8f\n", crealf(*_u), cimagf(*_v));
    return 1;
}

// compare roots for sorting
int POLY(_sort_roots_compare)(const void * _a,
                              const void * _b)
{
    double ar = (double) creal( *((TC*)_a) );
    double br = (double) creal( *((TC*)_b) );

    double ai = (double) cimag( *((TC*)_a) );
    double bi = (double) cimag( *((TC*)_b) );

    return ar == br ? (ai > bi ? -1 : 1) : (ar > br ? 1 : -1);
}

