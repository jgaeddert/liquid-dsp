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
#include "liquid.h"

//
// AUTOTEST: 
//
void autotest_decim_rrrf_generic()
{
    float h[] = {0.5, 0.5, 0.5, 0.5};
    unsigned int h_len = 4; // filter length
    unsigned int D = 4;     // decim factor
    decim_rrrf q = decim_rrrf_create(D,h,h_len);

    float x[16] = { 
        0.5,  0.5,  0.5,  0.5,
       -0.5, -0.5, -0.5, -0.5,
        0.5,  0.5,  0.5,  0.5,
        0.5,  0.5,  0.5,  0.5};

    float y[4];
    float test[4] = {1, -1, 1, 1};
    float tol = 1e-6f;

    unsigned int i, n=0;
    for (i=0; i<4; i++) {
        decim_rrrf_execute(q, &x[n], &y[i], D-1);
        n+=D;
    }   

    for (i=0; i<4; i++) {
        CONTEND_DELTA(y[i], test[i], tol);
        
        if (liquid_autotest_verbose)
            printf("  y(%u) = %8.4f;\n", i+1, y[i]);
    }

    decim_rrrf_destroy(q);
}

