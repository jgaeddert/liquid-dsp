/*
 * Copyright (c) 2011 Joseph Gaeddert
 * Copyright (c) 2011 Virginia Polytechnic Institute & State University
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
// Solve linear system of equations using conjugate gradient method
//

#include <string.h>

#include "liquid.internal.h"

#define DEBUG_CGSOLVE 0

// 
void MATRIX(_cgsolve)(T * _A,
                      unsigned int _n,
                      T * _b,
                      T * _x,
                      void * _opts)
{
    unsigned int i;
    unsigned int j;

    // TODO : validate input

    // TODO : check options
    //  1. set initial _x0
    //  2. max number of iterations
    //  3. residual tolerance

    // allocate memory for arrays
    T x0[_n], x1[_n];
    T d0[_n], d1[_n];
    T r0[_n], r1[_n];
    T alpha;
    T beta;

    // initialize arrays

    // 
    for (j=0; j<_n; j++)
        x0[j] = 0.0;

    // d_0 = b - A*x(0)
    // assume x(0) = {0, 0, 0, ...0}
    for (j=0; j<_n; j++) {
        d0[j] = _b[j];
        r0[j] = d0[j];
    }

    for (i=0; i<_n; i++) {
#if DEBUG_CGSOLVE
        printf("*********** %u **************\n", i);
#endif

        // step size
        T t0 = 0.0;
        for (j=0; j<_n; j++)
            t0 += r0[j] * conjf(r0[j]);
        T t1[_n];
        MATRIX(_mul)(_A, _n, _n,
                     d0, _n,  1,
                     t1, _n,  1);
        T t2;
        MATRIX(_mul)(d0,  1, _n,
                     t1, _n,  1,
                     &t2, 1,  1);
        alpha = t0 / t2;
#if DEBUG_CGSOLVE
        printf("  alpha  = %12.8f (t0:%12.8f, t2:%12.8f)\n", crealf(alpha), crealf(t0), crealf(t2));
#endif

        // update x
        for (j=0; j<_n; j++)
            x1[j] = x0[j] - alpha*d0[j];

#if DEBUG_CGSOLVE
        for (j=0; j<_n; j++)
            printf("  x[%3u] = %12.8f\n", j, crealf(x1[j]));
#endif

        // update r
        T t3[_n];
        MATRIX(_mul)(_A, _n, _n,
                     d0, _n,  1,
                     t3, _n,  1);
        for (j=0; j<_n; j++)
            r1[j] = r0[j] - alpha*t3[j];

        // update beta
        T t4 = 0.0;
        for (j=0; j<_n; j++)
            t4 += r1[j] * conjf(r1[j]);
        beta = t4 / t0;

        // update d
        for (j=0; j<_n; j++)
            d1[j] = r1[j] + beta*d0[j];

        // copy old x, d, r
        memmove(x0, x1, _n*sizeof(T));
        memmove(d0, d1, _n*sizeof(T));
        memmove(r0, r1, _n*sizeof(T));
    }

    // copy result to output
    memmove(_x, x0, _n*sizeof(T));
}
