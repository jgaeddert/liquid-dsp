/*
 * Copyright (c) 2012 Joseph Gaeddert
 * Copyright (c) 2012 Virginia Polytechnic Institute & State University
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
#include "liquid.internal.h"

// 
// AUTOTEST: compare structured result to oridinal computation
//

// helper function (compare structured object to ordinal computation)
void runtest_dotprod_cccf(unsigned int _n)
{
    float tol = 1e-3;
    float complex h[_n];
    float complex x[_n];

    // generate random coefficients
    unsigned int i;
    for (i=0; i<_n; i++) {
        h[i] = randnf() + randnf() * _Complex_I;
        x[i] = randnf() + randnf() * _Complex_I;
    }
    
    // compute expected value (ordinal computation)
    float complex y_test;
    dotprod_cccf_run(h, x, _n, &y_test);

    // create and run dot product object
    float complex y;
    dotprod_cccf dp;
    dp = dotprod_cccf_create(h,_n);
    dotprod_cccf_execute(dp, x, &y);
    dotprod_cccf_destroy(dp);

    // print results
    if (liquid_autotest_verbose) {
        printf("  dotprod-cccf-%-4u : %12.8f + j%12.8f (expected %12.8f + j%12.8f)\n",
                _n, crealf(y), cimagf(y), crealf(y_test), cimagf(y_test));
    }

    // validate result
    CONTEND_DELTA(crealf(y), crealf(y_test), tol);
    CONTEND_DELTA(cimagf(y), cimagf(y_test), tol);
}

// compare structured object to ordinal computation
void autotest_dotprod_cccf_struct_vs_ordinal()
{
    // run many, many tests
    unsigned int i;
    for (i=1; i<=512; i++)
        runtest_dotprod_cccf(i);
}

