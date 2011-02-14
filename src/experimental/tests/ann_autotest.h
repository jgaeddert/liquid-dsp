/*
 * Copyright (c) 2007, 2009, 2011 Joseph Gaeddert
 * Copyright (c) 2007, 2009, 2011 Virginia Polytechnic
 *                                Institute & State University
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
// AUTOTEST: ann node
//
void xautotest_ann_node()
{
    float w[4] = {1,2,3,4}; // weights vector
    float x[3] = {5,1,3};   // input vector
    float y[1];             // output vector

    // create node
    node q = node_create(w,     // weights
                         x,     // input
                         y,     // output
                         3,     // num_inputs
                         0,     // activation function ID
                         1.0f   // activation function gain
                        );

    // evaluate node
    node_evaluate(q);

    if (liquid_autotest_verbose) {
        node_print(q);
        printf("y = %12.8f\n", y[0]);
    }

    // contend equality of output
    CONTEND_EQUALITY(y[0], 20.0f);

    // destroy node
    node_destroy(q);
}


