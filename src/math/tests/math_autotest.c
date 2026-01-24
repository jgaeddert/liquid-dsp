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
#include "liquid.internal.h"


// Q function
void autotest_Q()
{
    float tol = 1e-6f;
    CONTEND_DELTA(liquid_Qf(-4.0f), 0.999968329f, tol);
    CONTEND_DELTA(liquid_Qf(-3.0f), 0.998650102f, tol);
    CONTEND_DELTA(liquid_Qf(-2.0f), 0.977249868f, tol);
    CONTEND_DELTA(liquid_Qf(-1.0f), 0.841344746f, tol);
    CONTEND_DELTA(liquid_Qf( 0.0f), 0.5f,         tol);
    CONTEND_DELTA(liquid_Qf( 1.0f), 0.158655254f, tol);
    CONTEND_DELTA(liquid_Qf( 2.0f), 0.022750132f, tol);
    CONTEND_DELTA(liquid_Qf( 3.0f), 0.001349898f, tol);
    CONTEND_DELTA(liquid_Qf( 4.0f), 0.000031671f, tol);
}

// Marcum Q function with M=1
void autotest_MarcumQ1f()
{
    float tol = 1e-6f;
    CONTEND_DELTA(liquid_MarcumQ1f(1.0, 2.0), 0.269012060035909996678, tol);
    CONTEND_DELTA(liquid_MarcumQ1f(4.0, 2.0), 0.985276535891284799866, tol);
    CONTEND_DELTA(liquid_MarcumQ1f(0.7, 1.2), 0.566189865978391927421, tol);
}

// Marcum Q function
void xautotest_MarcumQf()
{
    //float tol = 1e-6f;
    //CONTEND_DELTA(liquid_MarcumQf(1,1.0,2.0), 0.26901206003590999667, tol);
    //CONTEND_DELTA(liquid_MarcumQf(1,4.0,2.0), 0.98527653589128479986, tol);
    //CONTEND_DELTA(liquid_MarcumQf(2,0.7,1.2), 0.86541357815831636702, tol);
}

// sincf
void autotest_sincf()
{
    float tol = 1e-3f;
    CONTEND_DELTA(sincf(-2.0f), 0.0f, tol);
    CONTEND_DELTA(sincf(-1.0f), 0.0f, tol);
    CONTEND_DELTA(sincf( 0.0f), 1.0f, tol);
    CONTEND_DELTA(sincf( 1.0f), 0.0f, tol);
    CONTEND_DELTA(sincf( 2.0f), 0.0f, tol);

    CONTEND_DELTA(sincf(-0.1f), 0.983631643083466, tol);
    CONTEND_DELTA(sincf( 0.1f), 0.983631643083466, tol);
    CONTEND_DELTA(sincf( 3.1f),-0.031730053002692, tol);
    CONTEND_DELTA(sincf( 6.5f), 0.048970751720583, tol);
}

// nextpow2
void autotest_nextpow2()
{
    CONTEND_EQUALITY(liquid_nextpow2(1),    0);

    CONTEND_EQUALITY(liquid_nextpow2(2),    1);

    CONTEND_EQUALITY(liquid_nextpow2(3),    2);
    CONTEND_EQUALITY(liquid_nextpow2(4),    2);

    CONTEND_EQUALITY(liquid_nextpow2(5),    3);
    CONTEND_EQUALITY(liquid_nextpow2(6),    3);
    CONTEND_EQUALITY(liquid_nextpow2(7),    3);
    CONTEND_EQUALITY(liquid_nextpow2(8),    3);

    CONTEND_EQUALITY(liquid_nextpow2(9),    4);
    CONTEND_EQUALITY(liquid_nextpow2(10),   4);
    CONTEND_EQUALITY(liquid_nextpow2(11),   4);
    CONTEND_EQUALITY(liquid_nextpow2(12),   4);
    CONTEND_EQUALITY(liquid_nextpow2(13),   4);
    CONTEND_EQUALITY(liquid_nextpow2(14),   4);
    CONTEND_EQUALITY(liquid_nextpow2(15),   4);

    CONTEND_EQUALITY(liquid_nextpow2(67),   7);
    CONTEND_EQUALITY(liquid_nextpow2(179),  8);
    CONTEND_EQUALITY(liquid_nextpow2(888),  10);
}

// test math configuration and error handling
void autotest_math_config()
{
    _liquid_error_downgrade_enable();

    CONTEND_EQUALITY(liquid_nextpow2(0), 0);

    CONTEND_EQUALITY(liquid_nchoosek(4, 5), 0.0f);

    CONTEND_EQUALITY(liquid_lngammaf(-1), 0.0f);

    CONTEND_EQUALITY(liquid_gcd(12, 0), 0);
    CONTEND_EQUALITY(liquid_gcd( 0,12), 0);
    CONTEND_EQUALITY(liquid_gcd( 0, 0), 0);

    CONTEND_EQUALITY(liquid_MarcumQ1f( 0.0f, 2.0f), 0.0f); // alpha cannot be 0
    CONTEND_EQUALITY(liquid_MarcumQ1f(-1.0f, 2.0f), 0.0f); // alpha cannot be negative
    CONTEND_EQUALITY(liquid_MarcumQ1f( 1.0f,-1.0f), 0.0f); // beta cannot be negative

    _liquid_error_downgrade_disable();
}

