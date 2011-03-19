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

#include "autotest/autotest.h"
#include "liquid.experimental.h"

// 
// AUTOTEST: AWGN channel
//
void autotest_awgn_channel()
{
    unsigned long int N=100000; // number of trials
    unsigned long int i;
    float complex x;
    float m1=0.0f, m2=0.0f;
    float nvar = 3.0f;
    float tol=0.05f;

    awgn_channel q = awgn_channel_create(nvar);

    // uniform
    for (i=0; i<N; i++) {
        awgn_channel_execute(q, 0.0, &x);
        m1 += crealf(x) + cimagf(x);
        m2 += crealf(x)*crealf(x) + cimagf(x)*cimagf(x);
    }
    N *= 2; // double N for real and imag components
    m1 /= (float) N;
    m2 = (m2 / (float)N) - m1*m1;

    if (liquid_autotest_verbose) {
        printf("awgn mean       :   %12.8f (expected %12.8f)\n", m1, 0.0f);
        printf("awgn variance   :   %12.8f (expected %12.8f)\n", m2, nvar);
    }

    CONTEND_DELTA(m1, 0.0f, tol);
    CONTEND_DELTA(m2, nvar, tol);

    awgn_channel_destroy(q);
}

