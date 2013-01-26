/*
 * Copyright (c) 2013 Joseph Gaeddert
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

#include "autotest/autotest.h"
#include "liquid.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>

#include "liquid.h"

//
// AUTOTEST: 
//
void autotest_gradsearch_rosenbrock()
{
    float tol = 1e-3f;                  // error tolerance
    unsigned int num_parameters = 6;    // dimensionality of search (minimum 2)
    unsigned int num_iterations = 4000; // number of iterations to run

    // initialize vector for optimization
    float v_opt[num_parameters];
    unsigned int i;
    for (i=0; i<num_parameters; i++)
        v_opt[i] = 0.0f;

    // create gradsearch object
    gradsearch gs = gradsearch_create(NULL,
                                      v_opt,
                                      num_parameters,
                                      liquid_rosenbrock,
                                      LIQUID_OPTIM_MINIMIZE);

#if 0
    // execute search
    float u_opt = gradsearch_execute(gs, num_iterations, -1e-6f);
#else
    // execute search one iteration at a time
    unsigned int d=1;
    for (i=0; i<num_iterations; i++) {
        gradsearch_step(gs);

        // periodically print updates
        if (liquid_autotest_verbose) {
            if (((i+1)%d)==0 || i==0 || i == num_iterations-1) {
                printf("%5u: ", i+1);
                gradsearch_print(gs);

                if ((i+1)==10*d) d*=10;
            }
        }
    }
#endif

    // destroy gradient descent search object
    gradsearch_destroy(gs);

    // test results, optimum at [1, 1, 1, ... 1];
    for (i=0; i<num_parameters; i++)
        CONTEND_DELTA(v_opt[i], 1.0f, tol);

    // test value of utility (should be nearly 0)
    CONTEND_DELTA( liquid_rosenbrock(NULL, v_opt, num_parameters), 0.0f, tol );
}

