/*
 * Copyright (c) 2011 Joseph Gaeddert
 * Copyright (c) 2011 Virginia Polytechnic Institute & State University
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
#include "liquid.h"

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

