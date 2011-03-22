/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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

#ifndef __MATH_AUTOTEST_H__
#define __MATH_AUTOTEST_H__

#include "autotest/autotest.h"
#include "liquid.h"

// 
// AUTOTEST: Gamma
//
void autotest_gamma()
{
    float tol = 1e-3f;
    //CONTEND_DELTA(liquid_gammaf(0.1f), 9.51350769866873f, tol);
    CONTEND_DELTA(liquid_gammaf(0.2f), 4.59084371199880f, tol);
    CONTEND_DELTA(liquid_gammaf(0.5f), 1.77245385090552f, tol);
    CONTEND_DELTA(liquid_gammaf(1.5f), 0.886226925452758f, tol);
    CONTEND_DELTA(liquid_gammaf(2.5f), 1.329340388179140f, tol);
    CONTEND_DELTA(liquid_gammaf(3.2f), 2.42396547993537f, tol);
    CONTEND_DELTA(liquid_gammaf(4.1f), 6.81262286301667f, tol);
    CONTEND_DELTA(liquid_gammaf(5.3f), 38.0779764499523f, tol);
}

// 
// AUTOTEST: upper incomplete Gamma
//
void autotest_uppergamma()
{
    float tol = 1e-3f;

    CONTEND_DELTA(liquid_uppergammaf(2.1f, 0.001f), 1.04649f,  tol);

    CONTEND_DELTA(liquid_uppergammaf(2.1f, 0.01f),  1.04646f,  tol);

    CONTEND_DELTA(liquid_uppergammaf(2.1f, 0.1f),   1.04295f,  tol);
    CONTEND_DELTA(liquid_uppergammaf(2.1f, 0.2f),   1.03231f,  tol);
    CONTEND_DELTA(liquid_uppergammaf(2.1f, 0.3f),   1.01540f,  tol);
    CONTEND_DELTA(liquid_uppergammaf(2.1f, 0.4f),   0.993237f, tol);
    CONTEND_DELTA(liquid_uppergammaf(2.1f, 0.5f),   0.966782f, tol);
    CONTEND_DELTA(liquid_uppergammaf(2.1f, 0.6f),   0.936925f, tol);
    CONTEND_DELTA(liquid_uppergammaf(2.1f, 0.7f),   0.904451f, tol);
    CONTEND_DELTA(liquid_uppergammaf(2.1f, 0.8f),   0.870053f, tol);
    CONTEND_DELTA(liquid_uppergammaf(2.1f, 0.9f),   0.834330f, tol);
    CONTEND_DELTA(liquid_uppergammaf(2.1f, 1.0f),   0.797796f, tol);

    CONTEND_DELTA(liquid_uppergammaf(2.1f, 2.0f),   0.455589f, tol);
    CONTEND_DELTA(liquid_uppergammaf(2.1f, 3.0f),   0.229469f, tol);
    CONTEND_DELTA(liquid_uppergammaf(2.1f, 4.0f),   0.107786f, tol);
    CONTEND_DELTA(liquid_uppergammaf(2.1f, 5.0f),   0.0484292f, tol);
    CONTEND_DELTA(liquid_uppergammaf(2.1f, 6.0f),   0.0211006f, tol);
    CONTEND_DELTA(liquid_uppergammaf(2.1f, 7.0f),   0.00898852f, tol);
    CONTEND_DELTA(liquid_uppergammaf(2.1f, 8.0f),   0.00376348f, tol);
    CONTEND_DELTA(liquid_uppergammaf(2.1f, 9.0f),   0.00155445f, tol);
    CONTEND_DELTA(liquid_uppergammaf(2.1f, 10.0f),  0.000635002f, tol);
}

// 
// AUTOTEST: Factorial
//
void autotest_factorial()
{
    float tol = 1e-3f;
    CONTEND_DELTA(liquid_factorialf(0), 1,   tol);
    CONTEND_DELTA(liquid_factorialf(1), 1,   tol);
    CONTEND_DELTA(liquid_factorialf(2), 2,   tol);
    CONTEND_DELTA(liquid_factorialf(3), 6,   tol);
    CONTEND_DELTA(liquid_factorialf(4), 24,  tol);
    CONTEND_DELTA(liquid_factorialf(5), 120, tol);
    CONTEND_DELTA(liquid_factorialf(6), 720, tol);
}

// 
// AUTOTEST: nchoosek
//
void autotest_nchoosek()
{
    float tol = 1e-3f;

    // nchoosek(6, k)
    CONTEND_DELTA(liquid_nchoosek(6,    0),      1,     tol);
    CONTEND_DELTA(liquid_nchoosek(6,    1),      6,     tol);
    CONTEND_DELTA(liquid_nchoosek(6,    2),     15,     tol);
    CONTEND_DELTA(liquid_nchoosek(6,    3),     20,     tol);
    CONTEND_DELTA(liquid_nchoosek(6,    4),     15,     tol);
    CONTEND_DELTA(liquid_nchoosek(6,    5),      6,     tol);
    CONTEND_DELTA(liquid_nchoosek(6,    6),      1,     tol);

    // nchoosek(7, k)
    CONTEND_DELTA(liquid_nchoosek(7,    0),      1,     tol);
    CONTEND_DELTA(liquid_nchoosek(7,    1),      7,     tol);
    CONTEND_DELTA(liquid_nchoosek(7,    2),     21,     tol);
    CONTEND_DELTA(liquid_nchoosek(7,    3),     35,     tol);
    CONTEND_DELTA(liquid_nchoosek(7,    4),     35,     tol);
    CONTEND_DELTA(liquid_nchoosek(7,    5),     21,     tol);
    CONTEND_DELTA(liquid_nchoosek(7,    6),      7,     tol);
    CONTEND_DELTA(liquid_nchoosek(7,    7),      1,     tol);

    // test very large numbers
    CONTEND_DELTA(liquid_nchoosek(124,  5),     225150024,  5000);
}

// 
// AUTOTEST: Bessel function of the first kind
//
void autotest_besselj()
{
    float tol = 1e-3f;

    // check case when nu=0
    CONTEND_DELTA(liquid_besselj(0.0f,0.0f),  1.000000000000000, tol);
    CONTEND_DELTA(liquid_besselj(0.0f,0.1f),  0.997501562066040, tol);
    CONTEND_DELTA(liquid_besselj(0.0f,0.2f),  0.990024972239576, tol);
    CONTEND_DELTA(liquid_besselj(0.0f,0.5f),  0.938469807240813, tol);
    CONTEND_DELTA(liquid_besselj(0.0f,1.0f),  0.765197686557967, tol);
    CONTEND_DELTA(liquid_besselj(0.0f,2.0f),  0.223890779141236, tol);
    CONTEND_DELTA(liquid_besselj(0.0f,3.0f), -0.260051954901934, tol);
    CONTEND_DELTA(liquid_besselj(0.0f,4.0f), -0.397149809863847, tol);
    CONTEND_DELTA(liquid_besselj(0.0f,6.0f),  0.150645257250997, tol);
    CONTEND_DELTA(liquid_besselj(0.0f,8.0f),  0.171650807137554, tol);

    // check case when nu=0.5
    CONTEND_DELTA(liquid_besselj(0.5f,0.0f),  0.000000000000000, tol);
    CONTEND_DELTA(liquid_besselj(0.5f,0.1f),  0.251892940326001, tol);
    CONTEND_DELTA(liquid_besselj(0.5f,0.2f),  0.354450744211402, tol);
    CONTEND_DELTA(liquid_besselj(0.5f,0.5f),  0.540973789934529, tol);
    CONTEND_DELTA(liquid_besselj(0.5f,1.0f),  0.671396707141804, tol);
    CONTEND_DELTA(liquid_besselj(0.5f,2.0f),  0.513016136561828, tol);
    CONTEND_DELTA(liquid_besselj(0.5f,3.0f),  0.065008182877376, tol);
    CONTEND_DELTA(liquid_besselj(0.5f,4.0f), -0.301920513291637, tol);
    CONTEND_DELTA(liquid_besselj(0.5f,6.0f), -0.091015409523068, tol);
    CONTEND_DELTA(liquid_besselj(0.5f,8.0f),  0.279092808570990, tol);

    // check case when nu=1.7
    CONTEND_DELTA(liquid_besselj(1.7f,0.0f),  0.000000000000000, tol);
    CONTEND_DELTA(liquid_besselj(1.7f,0.1f),  0.003971976455203, tol);
    CONTEND_DELTA(liquid_besselj(1.7f,0.2f),  0.012869169735073, tol);
    CONTEND_DELTA(liquid_besselj(1.7f,0.5f),  0.059920175825578, tol);
    CONTEND_DELTA(liquid_besselj(1.7f,1.0f),  0.181417665056645, tol);
    CONTEND_DELTA(liquid_besselj(1.7f,2.0f),  0.437811462130677, tol);
    CONTEND_DELTA(liquid_besselj(1.7f,3.0f),  0.494432522734784, tol);
    CONTEND_DELTA(liquid_besselj(1.7f,4.0f),  0.268439400467270, tol);
    CONTEND_DELTA(liquid_besselj(1.7f,6.0f), -0.308175744215833, tol);
    CONTEND_DELTA(liquid_besselj(1.7f,8.0f), -0.001102600927987, tol);
}

// 
// AUTOTEST: Bessel function of the first kind
//
void autotest_besselj_0()
{
    float tol = 1e-3f;
    CONTEND_DELTA(liquid_besselj_0(0.0f),  1.0f, tol);
    CONTEND_DELTA(liquid_besselj_0(0.1f),  0.997501562066040f, tol);
    CONTEND_DELTA(liquid_besselj_0(0.2f),  0.990024972239576f, tol);
    CONTEND_DELTA(liquid_besselj_0(0.5f),  0.938469807240813f, tol);
    CONTEND_DELTA(liquid_besselj_0(1.0f),  0.765197686557967f, tol);
    CONTEND_DELTA(liquid_besselj_0(2.0f),  0.223890779141236f, tol);
    CONTEND_DELTA(liquid_besselj_0(2.5f), -0.048383776468199f, tol);
    CONTEND_DELTA(liquid_besselj_0(3.0f), -0.260051954901934f, tol);
    CONTEND_DELTA(liquid_besselj_0(3.5f), -0.380127739987263f, tol);
    CONTEND_DELTA(liquid_besselj_0(4.0f), -0.397149809863848f, tol);
    CONTEND_DELTA(liquid_besselj_0(4.5f), -0.320542508985121f, tol);
}

// 
// AUTOTEST: Modified Bessel function of the first kind
//
void autotest_besseli()
{
    float tol = 1e-3f;

    // check case when nu=0
    CONTEND_DELTA(liquid_besseli(0.0f,0.0f), 1.0f, tol);
    CONTEND_DELTA(liquid_besseli(0.0f,0.1f), 1.00250156293410f, tol);
    CONTEND_DELTA(liquid_besseli(0.0f,0.2f), 1.01002502779515f, tol);
    CONTEND_DELTA(liquid_besseli(0.0f,0.5f), 1.06348337074132f, tol);
    CONTEND_DELTA(liquid_besseli(0.0f,1.0f), 1.26606587775201f, tol);
    CONTEND_DELTA(liquid_besseli(0.0f,2.0f), 2.27958530233607f, tol);
    CONTEND_DELTA(liquid_besseli(0.0f,3.0f), 4.88079258586503f, tol);

    // check case when nu=0.5
    CONTEND_DELTA(liquid_besseli(0.5f,0.0f), 0.000000000000000, tol);
    CONTEND_DELTA(liquid_besseli(0.5f,0.1f), 0.252733984600132, tol);
    CONTEND_DELTA(liquid_besseli(0.5f,0.2f), 0.359208417583362, tol);
    CONTEND_DELTA(liquid_besseli(0.5f,0.5f), 0.587993086790417, tol);
    CONTEND_DELTA(liquid_besseli(0.5f,1.0f), 0.937674888245489, tol);
    CONTEND_DELTA(liquid_besseli(0.5f,2.0f), 2.046236863089057, tol);
    CONTEND_DELTA(liquid_besseli(0.5f,3.0f), 4.614822903407577, tol);

    // check case when nu=1.3
    CONTEND_DELTA(liquid_besseli(1.3f,0.0f), 0.000000000000000, tol);
    CONTEND_DELTA(liquid_besseli(1.3f,0.1f), 0.017465030873157, tol);
    CONTEND_DELTA(liquid_besseli(1.3f,0.2f), 0.043144293848607, tol);
    CONTEND_DELTA(liquid_besseli(1.3f,0.5f), 0.145248507279042, tol);
    CONTEND_DELTA(liquid_besseli(1.3f,1.0f), 0.387392350983796, tol);
    CONTEND_DELTA(liquid_besseli(1.3f,2.0f), 1.290819215135879, tol);
    CONTEND_DELTA(liquid_besseli(1.3f,3.0f), 3.450680420553085, tol);
}

// 
// AUTOTEST: Modified Bessel function of the first kind
//
void autotest_besseli_0()
{
    float tol = 1e-3f;
    CONTEND_DELTA(liquid_besseli_0(0.0f), 1.0f, tol);
    CONTEND_DELTA(liquid_besseli_0(0.1f), 1.00250156293410f, tol);
    CONTEND_DELTA(liquid_besseli_0(0.2f), 1.01002502779515f, tol);
    CONTEND_DELTA(liquid_besseli_0(0.5f), 1.06348337074132f, tol);
    CONTEND_DELTA(liquid_besseli_0(1.0f), 1.26606587775201f, tol);
    CONTEND_DELTA(liquid_besseli_0(2.0f), 2.27958530233607f, tol);
    CONTEND_DELTA(liquid_besseli_0(3.0f), 4.88079258586503f, tol);
}


// 
// AUTOTEST: Q function
//
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

// 
// AUTOTEST: sincf
//
void autotest_sincf()
{
    float tol = 1e-3f;
    CONTEND_DELTA(sincf(0.0f), 1.0f, tol);
}

// 
// AUTOTEST: nextpow2
//
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

#endif 

