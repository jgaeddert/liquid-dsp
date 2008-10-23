#ifndef __MATH_AUTOTEST_H__
#define __MATH_AUTOTEST_H__

#include "../../../autotest/autotest.h"
#include "../src/math.h"

// 
// AUTOTEST: Gamma
//
void xautotest_gamma()
{
    float tol = 1e-3f;
    CONTEND_DELTA(gammaf(0.1f), 9.51350769866873f, tol);
    CONTEND_DELTA(gammaf(0.2f), 4.59084371199880f, tol);
    CONTEND_DELTA(gammaf(0.5f), 1.77245385090552f, tol);
    CONTEND_DELTA(gammaf(1.5f), 0.886226925452758f, tol);
    CONTEND_DELTA(gammaf(2.5f), 1.329340388179140f, tol);
}

// 
// AUTOTEST: Factorial
//
void autotest_factorial()
{
    float tol = 1e-3f;
    CONTEND_DELTA(factorialf(0), 1,   tol);
    CONTEND_DELTA(factorialf(1), 1,   tol);
    CONTEND_DELTA(factorialf(2), 2,   tol);
    CONTEND_DELTA(factorialf(3), 6,   tol);
    CONTEND_DELTA(factorialf(4), 24,  tol);
    CONTEND_DELTA(factorialf(5), 120, tol);
    CONTEND_DELTA(factorialf(6), 720, tol);
}

// 
// AUTOTEST: Bessel function of the first kind
//
void autotest_besselj_0()
{
    float tol = 1e-3f;
    CONTEND_DELTA(besselj_0(0.0f),  1.0f, tol);
    CONTEND_DELTA(besselj_0(0.1f),  0.997501562066040f, tol);
    CONTEND_DELTA(besselj_0(0.2f),  0.990024972239576f, tol);
    CONTEND_DELTA(besselj_0(0.5f),  0.938469807240813f, tol);
    CONTEND_DELTA(besselj_0(1.0f),  0.765197686557967f, tol);
    CONTEND_DELTA(besselj_0(2.0f),  0.223890779141236f, tol);
    CONTEND_DELTA(besselj_0(2.5f), -0.048383776468199f, tol);
    CONTEND_DELTA(besselj_0(3.0f), -0.260051954901934f, tol);
    CONTEND_DELTA(besselj_0(3.5f), -0.380127739987263f, tol);
    CONTEND_DELTA(besselj_0(4.0f), -0.397149809863848f, tol);
    CONTEND_DELTA(besselj_0(4.5f), -0.320542508985121f, tol);
}

// 
// AUTOTEST: Modified Bessel function of the first kind
//
void autotest_besseli_0()
{
    float tol = 1e-3f;
    CONTEND_DELTA(besseli_0(0.0f), 1.0f, tol);
    CONTEND_DELTA(besseli_0(0.1f), 1.00250156293410f, tol);
    CONTEND_DELTA(besseli_0(0.2f), 1.01002502779515f, tol);
    CONTEND_DELTA(besseli_0(0.5f), 1.06348337074132f, tol);
    CONTEND_DELTA(besseli_0(1.0f), 1.26606587775201f, tol);
    CONTEND_DELTA(besseli_0(2.0f), 2.27958530233607f, tol);
    CONTEND_DELTA(besseli_0(3.0f), 4.88079258586503f, tol);
}

#endif 

