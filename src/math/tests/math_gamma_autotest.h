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
// AUTOTEST: Gamma
//
void autotest_gamma()
{
    // error tolerance
    float tol = 1e-5f;

    // test vectors
    float v[12][2] = {
        {0.0001f, 9999.42288323161f     },
        {0.001f,   999.423772484595f    },
        {0.01f,     99.4325851191505f   },
        {0.1f,       9.51350769866873f  },
        {0.2f,       4.59084371199880f  },
        {0.5f,       1.77245385090552f  },
        {1.5f,       0.886226925452758f },
        {2.5f,       1.329340388179140f },
        {3.2f,       2.42396547993537f  },
        {4.1f,       6.81262286301667f  },
        {5.3f,      38.0779764499523f   },
        {12.0f, 39916800.0000000f       }};

    unsigned int i;
    for (i=0; i<12; i++) {
        // extract test vector
        float z = v[i][0];
        float g = v[i][1];

        // compute gamma
        float gamma = liquid_gammaf(z);

        // compute relative error
        float error = fabsf(gamma-g) / fabsf(g);

        // print results
        if (liquid_autotest_verbose)
            printf("  gamma(%12.4e) = %12.4e (expected %12.4e) %12.4e\n", z, gamma, g, error);

        // run test
        CONTEND_LESS_THAN(error, tol);
    }
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
    //CONTEND_DELTA(liquid_nchoosek(124,  5),     225150024,  5000);
}

