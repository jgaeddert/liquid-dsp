/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
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

