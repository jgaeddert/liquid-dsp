#ifndef __DOTPROD_AUTOTEST_H__
#define __DOTPROD_AUTOTEST_H__

#include "../../../autotest/autotest.h"
#include "../src/dotprod.h"

// 
// AUTOTEST: basic dot product
//
void autotest_fdotprod_basic()
{
    float tol = 1e-6;

    float h[16] = {
        1, -1, 1, -1, 1, -1, 1, -1,
        1, -1, 1, -1, 1, -1, 1, -1};

    float x0[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    float test0 = 0;
    CONTEND_DELTA(fdotprod_run(h,x0,16),  test0, tol);
    CONTEND_DELTA(fdotprod_run4(h,x0,16), test0, tol);

    float x1[16] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    float test1 = 0;
    CONTEND_DELTA(fdotprod_run(h,x1,16),  test1, tol);
    CONTEND_DELTA(fdotprod_run4(h,x1,16), test1, tol);

    float x2[16] = {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1};
    float test2 = -8;
    CONTEND_DELTA(fdotprod_run(h,x2,16),  test2, tol);
    CONTEND_DELTA(fdotprod_run4(h,x2,16), test2, tol);

    float x3[16] = {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0};
    float test3 = 8;
    CONTEND_DELTA(fdotprod_run(h,x3,16),  test3, tol);
    CONTEND_DELTA(fdotprod_run4(h,x3,16), test3, tol);

    float test4 = 16;
    CONTEND_DELTA(fdotprod_run(h,h,16),  test4, tol);
    CONTEND_DELTA(fdotprod_run4(h,h,16), test4, tol);

}

// 
// AUTOTEST: uneven dot product
//
void autotest_fdotprod_uneven()
{
    float tol = 1e-6;

    float h[16] = {
        1, -1, 1, -1, 1, -1, 1, -1,
        1, -1, 1, -1, 1, -1, 1, -1};

    float x[16] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

    float test1 = 1;
    CONTEND_DELTA(fdotprod_run(h,x,1),  test1, tol);
    CONTEND_DELTA(fdotprod_run4(h,x,1), test1, tol);

    float test2 = 0;
    CONTEND_DELTA(fdotprod_run(h,x,2),  test2, tol);
    CONTEND_DELTA(fdotprod_run4(h,x,2), test2, tol);

    float test3 = 1;
    CONTEND_DELTA(fdotprod_run(h,x,3),  test3, tol);
    CONTEND_DELTA(fdotprod_run4(h,x,3), test3, tol);

    float test11 = 1;
    CONTEND_DELTA(fdotprod_run(h,x,11),  test11, tol);
    CONTEND_DELTA(fdotprod_run4(h,x,11), test11, tol);

    float test13 = 1;
    CONTEND_DELTA(fdotprod_run(h,x,13),  test13, tol);
    CONTEND_DELTA(fdotprod_run4(h,x,13), test13, tol);

    float test15 = 1;
    CONTEND_DELTA(fdotprod_run(h,x,15),  test15, tol);
    CONTEND_DELTA(fdotprod_run4(h,x,15), test15, tol);

}

// 
// AUTOTEST: structured dot product
//
void autotest_fdotprod_struct()
{
    float tol = 1e-6;

    float h[16] = {
        1, -1, 1, -1, 1, -1, 1, -1,
        1, -1, 1, -1, 1, -1, 1, -1};

    // create object
    fdotprod dp = fdotprod_create(h,16);

    float x0[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    float test0 = 0;
    CONTEND_DELTA(fdotprod_execute(dp,x0),  test0, tol);

    float x1[16] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    float test1 = 0;
    CONTEND_DELTA(fdotprod_execute(dp,x1),  test1, tol);

    float x2[16] = {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1};
    float test2 = -8;
    CONTEND_DELTA(fdotprod_execute(dp,x2),  test2, tol);

    float *x3 = h;
    float test3 = 16;
    CONTEND_DELTA(fdotprod_execute(dp,x3),  test3, tol);

    // clean-up
    fdotprod_destroy(dp);
}

#endif 

