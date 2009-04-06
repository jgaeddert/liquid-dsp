#ifndef __LIQUID_FMATRIX_AUTOTEST_H__
#define __LIQUID_FMATRIX_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

// 
// AUTOTEST: Test fmatrix add
//
void autotest_fmatrix_add() {

    float x[6] = {
        1, 2, 3,
        4, 5, 6  };

    float y[6] = {
        0, 1, 2,
        3, 4, 5  };

    float z[6];
    float ztest[6] = {
        1, 3, 5,
        7, 9, 11 };

    fmatrix_add(2,3,x,y,z);

    CONTEND_SAME_DATA(z,ztest,sizeof(z));
}

// 
// AUTOTEST: Test fmatrix ops
//
void autotest_fmatrix_ops() {

    float x[6] = {
        1, 2, 3,
        4, 5, 6  };

    float y[9] = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9  };

    float z[6];
    float ztest[6] = {
        30, 36, 42,
        66, 81, 96   };

    fmatrix_mul(x,2,3, y,3,3, z,2,3);
    fmatrix_print(z,2,3);

    CONTEND_SAME_DATA(z,ztest,sizeof(z));
}

#endif // __LIQUID_FMATRIX_AUTOTEST_H__
