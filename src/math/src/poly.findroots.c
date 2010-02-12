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


// finds the complex roots of the polynomial
void POLY(_findroots)(T * _p,
                      unsigned int _k,
                      T * _roots)
{
    if (_k < 2) {
        fprintf(stderr,"%s_findroots(), order must be greater than 0\n", POLY_NAME);
        exit(1);
    }

    unsigned int i;
    unsigned int num_roots = _k-1;
    T r0[num_roots];
    T r1[num_roots];

    // initialize roots
    T t0 = 0.4f + 0.9f*_Complex_I;
    T t  = 1.0f;
    for (i=0; i<num_roots; i++) {
        r0[i] = t;
        t *= t0;
    }

    unsigned int num_iterations = 10;
    unsigned int j, k;
    T f;
    T fp;
    for (i=0; i<num_iterations; i++) {
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

        memmove(r0, r1, num_roots*sizeof(T));
    }

    memmove(_roots, r0, num_roots*sizeof(T));
}

