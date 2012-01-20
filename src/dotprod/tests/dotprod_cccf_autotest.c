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

// 
// AUTOTEST: dot product with floating-point data
//
void autotest_dotprod_cccf_rand16()
{
    float complex h[16] = {
      0.17702709 +   1.38978455*_Complex_I,  0.91294148 +   0.39217381*_Complex_I,
     -0.80607338 +   0.76477512*_Complex_I,  0.05099755 +  -0.87350051*_Complex_I,
      0.44513826 +  -0.49490569*_Complex_I,  0.14754967 +   2.04349962*_Complex_I,
      1.07246623 +   1.08146290*_Complex_I, -1.14028088 +   1.83380899*_Complex_I,
      0.38105361 +  -0.45591846*_Complex_I,  0.32605401 +   0.34440081*_Complex_I,
     -0.05477144 +   0.60832595*_Complex_I,  1.81667523 +  -1.12238075*_Complex_I,
     -0.87190497 +   1.10743858*_Complex_I,  1.30921403 +   1.24438643*_Complex_I,
      0.55524695 +  -1.94931519*_Complex_I, -0.87191170 +   0.91693119*_Complex_I,
    };

    float complex x[16] = {
     -2.19591953 +  -0.93229692*_Complex_I,  0.17150376 +   0.56165114*_Complex_I,
      1.58354529 +  -0.50696037*_Complex_I,  1.40929619 +   0.87868803*_Complex_I,
     -0.75505072 +  -0.30867372*_Complex_I, -0.09821367 +  -0.73949106*_Complex_I,
      0.03785571 +   0.72763665*_Complex_I, -1.20262636 +  -0.88838102*_Complex_I,
      0.23323685 +   0.12456235*_Complex_I,  0.34593736 +   0.02529594*_Complex_I,
      0.33669564 +   0.39064649*_Complex_I, -2.45003867 +  -0.54862205*_Complex_I,
     -2.64870707 +   2.33444473*_Complex_I, -0.92284477 +  -2.45121397*_Complex_I,
      0.24852918 +  -0.62409860*_Complex_I, -0.87039907 +   0.90921212*_Complex_I,
    };

    float complex y;
    float complex test = -0.604285042605890 - 12.390925785344704 * _Complex_I;

    float tol = 1e-3f;

    dotprod_cccf_run(h,x,16,&y);
    CONTEND_DELTA( crealf(y), crealf(test), tol);
    CONTEND_DELTA( cimagf(y), cimagf(test), tol);

    dotprod_cccf_run4(h,x,16,&y);
    CONTEND_DELTA( crealf(y), crealf(test), tol);
    CONTEND_DELTA( cimagf(y), cimagf(test), tol);

    // test object
    dotprod_cccf q = dotprod_cccf_create(h,16);
    dotprod_cccf_execute(q,x,&y);
    CONTEND_DELTA( crealf(y), crealf(test), tol);
    CONTEND_DELTA( cimagf(y), cimagf(test), tol);
    dotprod_cccf_destroy(q);
}

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

