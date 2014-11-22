/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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
// AUTOTEST: Find minimum of Rosenbrock function, should be [1 1 1 ...]
//
void autotest_gradsearch_rosenbrock()
{
    float tol = 1e-2f;                  // error tolerance
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

//
// AUTOTEST: Find maximum of: exp{ -sum{ (v[i]-1)^2/sigma_i^2 } }, should be [1 1 1 ...]
//

// test utility function
float utility_max_autotest(void *       _userdata,
                           float *      _v,
                           unsigned int _n)
{
    if (_n == 0) {
        fprintf(stderr,"error: liquid_invgauss(), input vector length cannot be zero\n");
        exit(1);
    }

    float t = 0.0f;
    float sigma = 1.0f;
    unsigned int i;
    for (i=0; i<_n; i++) {
        t += (_v[i]-1.0f)*(_v[i]-1.0f) / (sigma*sigma);

        // increase variance along this dimension
        sigma *= 1.5f;
    }

    return expf(-t);
}

void autotest_gradsearch_maxutility()
{
    float tol = 1e-2f;                  // error tolerance
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
                                      utility_max_autotest,
                                      LIQUID_OPTIM_MAXIMIZE);

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

    // destroy gradient descent search object
    gradsearch_destroy(gs);

    // test results, optimum at [1, 1, 1, ... 1];
    for (i=0; i<num_parameters; i++)
        CONTEND_DELTA(v_opt[i], 1.0f, tol);

    // test value of utility (should be nearly 1)
    CONTEND_DELTA( utility_max_autotest(NULL, v_opt, num_parameters), 1.0f, tol );
}

