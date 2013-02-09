/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2013 Joseph Gaeddert
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

#include <string.h>

#include "autotest/autotest.h"
#include "liquid.h"

// 
// AUTOTEST: Test matrixq16 add
//
void autotest_matrixq16_add() {

    q16_t x[6] = {
        q16_float_to_fixed(1), q16_float_to_fixed(2), q16_float_to_fixed(3),
        q16_float_to_fixed(4), q16_float_to_fixed(5), q16_float_to_fixed(6), };

    q16_t y[6] = {
        q16_float_to_fixed(0), q16_float_to_fixed(1), q16_float_to_fixed(2),
        q16_float_to_fixed(3), q16_float_to_fixed(4), q16_float_to_fixed(5), };

    q16_t z[6];
    float ztest[6] = {
        1, 3, 5,
        7, 9, 11 };

    matrixq16_add(x,y,z,2,3);

    unsigned int i;
    for (i=0; i<6; i++)
        CONTEND_DELTA( q16_fixed_to_float(z[i]), ztest[i], 1e-3f );
}

