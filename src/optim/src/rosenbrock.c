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
// rosenbrock.c : n-dimensional rosenbrock utility function
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "liquid.internal.h"

// n-dimensional Rosenbrock utility function, minimum at _v = {1,1,1...}
float rosenbrock(void * _userdata,
                 float * _v,
                 unsigned int _n)
{
    if (_n == 0) {
        fprintf(stderr,"error: rosenbrock(), input vector length cannot be zero\n");
        exit(1);
    } else if (_n == 1) {
        return (1.0f-_v[0])*(1.0f-_v[0]);
    }

    float u=0.0f;
    unsigned int i;
    for (i=0; i<_n-1; i++)
        u += powf(1-_v[i],2) + 100*powf( _v[i+1] - powf(_v[i],2), 2);

    return u;
}

