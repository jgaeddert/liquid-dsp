/*
 * Copyright (c) 2007 - 2026 Joseph Gaeddert
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

#include "liquid.autotest.h"
#include "liquid.internal.h"


LIQUID_AUTOTEST(Q,"Q function","",0.1)
{
    float tol = 1e-6f;
    LIQUID_CHECK_DELTA(liquid_Qf(-4.0f), 0.999968329f, tol);
    LIQUID_CHECK_DELTA(liquid_Qf(-3.0f), 0.998650102f, tol);
    LIQUID_CHECK_DELTA(liquid_Qf(-2.0f), 0.977249868f, tol);
    LIQUID_CHECK_DELTA(liquid_Qf(-1.0f), 0.841344746f, tol);
    LIQUID_CHECK_DELTA(liquid_Qf( 0.0f), 0.5f,         tol);
    LIQUID_CHECK_DELTA(liquid_Qf( 1.0f), 0.158655254f, tol);
    LIQUID_CHECK_DELTA(liquid_Qf( 2.0f), 0.022750132f, tol);
    LIQUID_CHECK_DELTA(liquid_Qf( 3.0f), 0.001349898f, tol);
    LIQUID_CHECK_DELTA(liquid_Qf( 4.0f), 0.000031671f, tol);
}

LIQUID_AUTOTEST(MarcumQf,"Marcum Q function, general","",0.1)
{
    //float tol = 1e-6f;
    //LIQUID_CHECK_DELTA(liquid_MarcumQf(1,1.0,2.0), 0.26901206003590999667, tol);
    //LIQUID_CHECK_DELTA(liquid_MarcumQf(1,4.0,2.0), 0.98527653589128479986, tol);
    //LIQUID_CHECK_DELTA(liquid_MarcumQf(2,0.7,1.2), 0.86541357815831636702, tol);
}

LIQUID_AUTOTEST(MarcumQ1f,"Marcum Q function with M=1","",0.1)
{
    float tol = 1e-6f;
    LIQUID_CHECK_DELTA(liquid_MarcumQ1f(1.0, 2.0), 0.269012060035909996678, tol);
    LIQUID_CHECK_DELTA(liquid_MarcumQ1f(4.0, 2.0), 0.985276535891284799866, tol);
    LIQUID_CHECK_DELTA(liquid_MarcumQ1f(0.7, 1.2), 0.566189865978391927421, tol);
}

LIQUID_AUTOTEST(sincf,"sincf","",0.1)
{
    float tol = 1e-3f;
    LIQUID_CHECK_DELTA(sincf(-2.0f), 0.0f, tol);
    LIQUID_CHECK_DELTA(sincf(-1.0f), 0.0f, tol);
    LIQUID_CHECK_DELTA(sincf( 0.0f), 1.0f, tol);
    LIQUID_CHECK_DELTA(sincf( 1.0f), 0.0f, tol);
    LIQUID_CHECK_DELTA(sincf( 2.0f), 0.0f, tol);

    LIQUID_CHECK_DELTA(sincf(-0.1f), 0.983631643083466, tol);
    LIQUID_CHECK_DELTA(sincf( 0.1f), 0.983631643083466, tol);
    LIQUID_CHECK_DELTA(sincf( 3.1f),-0.031730053002692, tol);
    LIQUID_CHECK_DELTA(sincf( 6.5f), 0.048970751720583, tol);
}

LIQUID_AUTOTEST(nextpow2,"next power of 2","",0.1)
{
    LIQUID_CHECK(liquid_nextpow2(1) ==     0);

    LIQUID_CHECK(liquid_nextpow2(2) ==     1);

    LIQUID_CHECK(liquid_nextpow2(3) ==     2);
    LIQUID_CHECK(liquid_nextpow2(4) ==     2);

    LIQUID_CHECK(liquid_nextpow2(5) ==     3);
    LIQUID_CHECK(liquid_nextpow2(6) ==     3);
    LIQUID_CHECK(liquid_nextpow2(7) ==     3);
    LIQUID_CHECK(liquid_nextpow2(8) ==     3);

    LIQUID_CHECK(liquid_nextpow2(9) ==     4);
    LIQUID_CHECK(liquid_nextpow2(10) ==    4);
    LIQUID_CHECK(liquid_nextpow2(11) ==    4);
    LIQUID_CHECK(liquid_nextpow2(12) ==    4);
    LIQUID_CHECK(liquid_nextpow2(13) ==    4);
    LIQUID_CHECK(liquid_nextpow2(14) ==    4);
    LIQUID_CHECK(liquid_nextpow2(15) ==    4);

    LIQUID_CHECK(liquid_nextpow2(67) ==    7);
    LIQUID_CHECK(liquid_nextpow2(179) ==   8);
    LIQUID_CHECK(liquid_nextpow2(888) ==   10);
}

LIQUID_AUTOTEST(math_config,"test math configuration and error handling","",0.1)
{
    _liquid_error_downgrade_enable();

    LIQUID_CHECK(liquid_nextpow2(0) ==  0);

    LIQUID_CHECK(liquid_nchoosek(4, 5) ==  0.0f);

    LIQUID_CHECK(liquid_lngammaf(-1) ==  0.0f);

    LIQUID_CHECK(liquid_gcd(12, 0) ==  0);
    LIQUID_CHECK(liquid_gcd( 0,12) ==  0);
    LIQUID_CHECK(liquid_gcd( 0, 0) ==  0);

    LIQUID_CHECK(liquid_MarcumQ1f( 0.0f, 2.0f) ==  0.0f); // alpha cannot be 0
    LIQUID_CHECK(liquid_MarcumQ1f(-1.0f, 2.0f) ==  0.0f); // alpha cannot be negative
    LIQUID_CHECK(liquid_MarcumQ1f( 1.0f,-1.0f) ==  0.0f); // beta cannot be negative

    _liquid_error_downgrade_disable();
}

