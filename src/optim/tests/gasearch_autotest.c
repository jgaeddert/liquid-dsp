/*
 * Copyright (c) 2007 - 2023 Joseph Gaeddert
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

#include "autotest/autotest.h"
#include "liquid.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>

#include "liquid.internal.h"

// peak callback function; value nearest {p, p, p, ...} where p = 1/sqrt(2)
float gasearch_autotest_peak_callback(void * _userdata, chromosome _c)
{
    unsigned int i, n = chromosome_get_num_traits(_c);
    float u     = 1.0f;
    float sig   = 0.2f;
    float p     = M_SQRT1_2;
    for (i=0; i<n; i++) {
        // extract chromosome values
        float v = chromosome_valuef(_c,i);
        float e = v - p;
        u *= exp(-e*e/(2*sig*sig));
    }
    return u;
}

// find values which maximize function
void autotest_gasearch_peak()
{
    unsigned int num_parameters     = 16;   // dimensionality of search (minimum 1)
    unsigned int bits_per_parameter =  6;   // parameter resolution
    unsigned int num_iterations     = 8000; // number of iterations to run
    unsigned int population_size    = 16;   // GA population size
    float        mutation_rate      = 0.2f; // GA mutation rate
    float        tol                = 0.1f; // error tolerance

    unsigned int i;
    float optimum_utility;

    // create prototype chromosome
    chromosome prototype = chromosome_create_basic(num_parameters, bits_per_parameter);

    // create gasearch object
    gasearch ga = gasearch_create_advanced(gasearch_autotest_peak_callback, NULL, prototype,
                    LIQUID_OPTIM_MAXIMIZE, population_size, mutation_rate);

    // execute search at once
    optimum_utility = gasearch_run(ga, num_iterations, 1e6f);

    // get optimum utility
    gasearch_getopt(ga, prototype, &optimum_utility);
    float v_opt[num_parameters];
    for (i=0; i<num_parameters; i++)
        v_opt[i] = chromosome_valuef(prototype, i);

    if (liquid_autotest_verbose) {
        gasearch_print(ga);
        printf(" opt: [  %6.4f] ", optimum_utility);
        chromosome_printf(prototype);
    }

    // destroy gradient descent search object
    chromosome_destroy(prototype);
    gasearch_destroy(ga);

    // test results, optimum at {p, p, p, ...} where p = 1/sqrt(2)
    for (i=0; i<num_parameters; i++)
        CONTEND_DELTA(v_opt[i], M_SQRT1_2, tol)

    // test value of utility (should be nearly 1)
    CONTEND_GREATER_THAN( optimum_utility, 0.70f )
}

