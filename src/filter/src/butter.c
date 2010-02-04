/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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
// Butterworth filter design
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "liquid.internal.h"

// butterworth polynomial
void butterpolyf(unsigned int _n, float *_p)
{
    
}

void butterf(float * _b,
             float * _a,
             unsigned int _n)
{
    // poles
    float complex s[_n];

    unsigned int i;
    float theta;
    for (i=0; i<_n; i++) {
        theta = (float)(2*(i+1) + _n - 1)*M_PI/(float)(2*_n);
        s[i] = -cexpf(_Complex_I*theta);
    }
    
    printf("poles:\n");
    for (i=0; i<_n; i++)
        printf("  s[%3u] = %12.8f + j*%12.8f\n", i+1, crealf(-s[i]), cimagf(-s[i]));

    // expand roots
    float complex p[_n+1];
    cfpoly_expandroots(s,_n,p);

    // print results
    printf("expanded polynomial:\n");
    for (i=0; i<=_n; i++)
        printf("  p[%3u] = %12.8f + j*%12.8f\n", i+1, crealf(p[i]), cimagf(p[i]));
}

