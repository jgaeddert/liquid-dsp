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

#endif 

