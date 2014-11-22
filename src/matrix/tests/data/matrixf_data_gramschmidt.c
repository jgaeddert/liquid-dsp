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

//
// data for testing Gram-Schmidt orthonormalization
//

#include <complex.h>

// matrixf_data_gramschmidt_A [size: 4 x 3]
float matrixf_data_gramschmidt_A[] = {
    1.000000000000 /* ( 0, 0) */,
    2.000000000000 /* ( 0, 1) */,
    1.000000000000 /* ( 0, 2) */,
    0.000000000000 /* ( 1, 0) */,
    2.000000000000 /* ( 1, 1) */,
    0.000000000000 /* ( 1, 2) */,
    2.000000000000 /* ( 2, 0) */,
    3.000000000000 /* ( 2, 1) */,
    1.000000000000 /* ( 2, 2) */,
    1.000000000000 /* ( 3, 0) */,
    1.000000000000 /* ( 3, 1) */,
    0.000000000000 /* ( 3, 2) */};

// matrixf_data_gramschmidt_V [size: 4 x 3]
float matrixf_data_gramschmidt_V[] = {
    0.408248290464 /* ( 0, 0) */,
    0.235702260396 /* ( 0, 1) */,
    0.666666666667 /* ( 0, 2) */,
    0.000000000000 /* ( 1, 0) */,
    0.942809041582 /* ( 1, 1) */,
   -0.333333333333 /* ( 1, 2) */,
    0.816496580928 /* ( 2, 0) */,
    0.000000000000 /* ( 2, 1) */,
    0.000000000000 /* ( 2, 2) */,
    0.408248290464 /* ( 3, 0) */,
   -0.235702260396 /* ( 3, 1) */,
   -0.666666666667 /* ( 3, 2) */};

