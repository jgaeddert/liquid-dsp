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

#include <string.h>

#include "autotest/autotest.h"
#include "liquid.internal.h"

// 
// AUTOTEST: basic dot product
//
void autotest_dotprod_rrrf_basic()
{
    float tol = 1e-6;   // error tolerance
    float y;            // return value

    float h[16] = {
        1, -1, 1, -1, 1, -1, 1, -1,
        1, -1, 1, -1, 1, -1, 1, -1};

    float x0[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    float test0 = 0;
    dotprod_rrrf_run(h,x0,16,&y);
    CONTEND_DELTA(y,  test0, tol);
    dotprod_rrrf_run4(h,x0,16,&y);
    CONTEND_DELTA(y,  test0, tol);

    float x1[16] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    float test1 = 0;
    dotprod_rrrf_run(h,x1,16,&y);
    CONTEND_DELTA(y,  test1, tol);
    dotprod_rrrf_run4(h,x1,16,&y);
    CONTEND_DELTA(y,  test1, tol);

    float x2[16] = {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1};
    float test2 = -8;
    dotprod_rrrf_run(h,x2,16,&y);
    CONTEND_DELTA(y,  test2, tol);
    dotprod_rrrf_run4(h,x2,16,&y);
    CONTEND_DELTA(y,  test2, tol);

    float x3[16] = {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0};
    float test3 = 8;
    dotprod_rrrf_run(h,x3,16,&y);
    CONTEND_DELTA(y,  test3, tol);
    dotprod_rrrf_run4(h,x3,16,&y);
    CONTEND_DELTA(y,  test3, tol);

    float test4 = 16;
    dotprod_rrrf_run(h,h,16,&y);
    CONTEND_DELTA(y,  test4, tol);
    dotprod_rrrf_run4(h,h,16,&y);
    CONTEND_DELTA(y,  test4, tol);

}

// 
// AUTOTEST: uneven dot product
//
void autotest_dotprod_rrrf_uneven()
{
    float tol = 1e-6;
    float y;

    float h[16] = {
        1, -1, 1, -1, 1, -1, 1, -1,
        1, -1, 1, -1, 1, -1, 1, -1};

    float x[16] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

    float test1 = 1;
    dotprod_rrrf_run(h,x,1,&y);
    CONTEND_DELTA(y,  test1, tol);

    float test2 = 0;
    dotprod_rrrf_run(h,x,2,&y);
    CONTEND_DELTA(y, test2, tol);

    float test3 = 1;
    dotprod_rrrf_run(h,x,3,&y);
    CONTEND_DELTA(y, test3, tol);

    float test11 = 1;
    dotprod_rrrf_run(h,x,11,&y);
    CONTEND_DELTA(y, test11, tol);

    float test13 = 1;
    dotprod_rrrf_run(h,x,13,&y);
    CONTEND_DELTA(y, test13, tol);

    float test15 = 1;
    dotprod_rrrf_run(h,x,15,&y);
    CONTEND_DELTA(y, test15, tol);

}

// 
// AUTOTEST: structured dot product
//
void autotest_dotprod_rrrf_struct()
{
    float tol = 1e-6;
    float y;

    float h[16] = {
        1, -1, 1, -1, 1, -1, 1, -1,
        1, -1, 1, -1, 1, -1, 1, -1};

    // create object
    dotprod_rrrf dp = dotprod_rrrf_create(h,16);

    float x0[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    float test0 = 0;
    dotprod_rrrf_execute(dp,x0,&y);
    CONTEND_DELTA(y,  test0, tol);

    float x1[16] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    float test1 = 0;
    dotprod_rrrf_execute(dp,x1,&y);
    CONTEND_DELTA(y,  test1, tol);

    float x2[16] = {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1};
    float test2 = -8;
    dotprod_rrrf_execute(dp,x2,&y);
    CONTEND_DELTA(y,  test2, tol);

    float *x3 = h;
    float test3 = 16;
    dotprod_rrrf_execute(dp,x3,&y);
    CONTEND_DELTA(y,  test3, tol);

    // clean-up
    dotprod_rrrf_destroy(dp);
}

// 
// AUTOTEST: structured dot product with floating-point data
//
void autotest_dotprod_rrrf_struct_align()
{
    float h[16] = {
    -0.050565, -0.952580,  0.274320,  1.232400, 
     1.268200,  0.565770,  0.800830,  0.923970, 
     0.517060, -0.530340, -0.378550, -1.127100, 
     1.123100, -1.006000, -1.483800, -0.062007
    };

    float x[16] = {
    -0.384280, -0.812030,  0.156930,  1.919500, 
     0.564580, -0.123610, -0.138640,  0.004984, 
    -1.100200, -0.497620,  0.089977, -1.745500, 
     0.463640,  0.592100,  1.150000, -1.225400
    };

    float test = 3.66411513609863;
    float tol = 1e-3f;
    float y;

    // create dotprod object
    dotprod_rrrf dp = dotprod_rrrf_create(h,16);

    // test data mis-alignment conditions
    float x_buffer[20];
    float * x_hat;
    unsigned int i;
    for (i=0; i<4; i++) {
        // set pointer to array aligned with counter
        x_hat = &x_buffer[i];

        // copy input data to buffer
        memmove(x_hat, x, 16*sizeof(float));
        
        // execute dotprod
        dotprod_rrrf_execute(dp,x_hat,&y);
        CONTEND_DELTA(y,test,tol);
    }

    // destroy dotprod object
    dotprod_rrrf_destroy(dp);
}


// 
// AUTOTEST: dot product with floating-point data
//
void autotest_dotprod_rrrf_rand01()
{
    float h[16] = {
    -0.050565, -0.952580,  0.274320,  1.232400, 
     1.268200,  0.565770,  0.800830,  0.923970, 
     0.517060, -0.530340, -0.378550, -1.127100, 
     1.123100, -1.006000, -1.483800, -0.062007
    };

    float x[16] = {
    -0.384280, -0.812030,  0.156930,  1.919500, 
     0.564580, -0.123610, -0.138640,  0.004984, 
    -1.100200, -0.497620,  0.089977, -1.745500, 
     0.463640,  0.592100,  1.150000, -1.225400
    };

    float test = 3.66411513609863;
    float tol = 1e-3f;
    float y;

    dotprod_rrrf_run(h,x,16,&y);
    CONTEND_DELTA(y,test,tol);
}

// 
// AUTOTEST: dot product with floating-point data
//
void autotest_dotprod_rrrf_rand02()
{
    float h[16] = {
     2.595300,  1.243600, -0.818550, -1.439800, 
     0.055795, -1.476000,  0.445900,  0.325460, 
    -3.451200,  0.058528, -0.246990,  0.476290, 
    -0.598780, -0.885250,  0.464660, -0.610140
    };

    float x[16] = {
    -0.917010, -1.278200, -0.533190,  2.309200, 
     0.592980,  0.964820,  0.183220, -0.082864, 
     0.057171, -1.186500, -0.738260,  0.356960, 
    -0.144000, -1.435200, -0.893420,  1.657800
    };

    float test = -8.17832326680587;
    float tol = 1e-3f;
    float y;

    dotprod_rrrf_run(h,x,16,&y);
    CONTEND_DELTA(y,test,tol);
}

