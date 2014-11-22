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
// data for testing Cholesky decomposition
//

#include <complex.h>

// matrixf_data_chol_L [size: 4 x 4]
float matrixf_data_chol_L[] = {
    1.010000000000 /* ( 0, 0) */,
    0.000000000000 /* ( 0, 1) */,
    0.000000000000 /* ( 0, 2) */,
    0.000000000000 /* ( 0, 3) */,
   -1.420000000000 /* ( 1, 0) */,
    0.500000000000 /* ( 1, 1) */,
    0.000000000000 /* ( 1, 2) */,
    0.000000000000 /* ( 1, 3) */,
    0.320000000000 /* ( 2, 0) */,
    2.010000000000 /* ( 2, 1) */,
    0.300000000000 /* ( 2, 2) */,
    0.000000000000 /* ( 2, 3) */,
   -1.020000000000 /* ( 3, 0) */,
   -0.320000000000 /* ( 3, 1) */,
   -1.650000000000 /* ( 3, 2) */,
    1.070000000000 /* ( 3, 3) */};

// matrixf_data_chol_A [size: 4 x 4]
float matrixf_data_chol_A[] = {
    1.020100000000 /* ( 0, 0) */,
   -1.434200000000 /* ( 0, 1) */,
    0.323200000000 /* ( 0, 2) */,
   -1.030200000000 /* ( 0, 3) */,
   -1.434200000000 /* ( 1, 0) */,
    2.266400000000 /* ( 1, 1) */,
    0.550600000000 /* ( 1, 2) */,
    1.288400000000 /* ( 1, 3) */,
    0.323200000000 /* ( 2, 0) */,
    0.550600000000 /* ( 2, 1) */,
    4.232500000000 /* ( 2, 2) */,
   -1.464600000000 /* ( 2, 3) */,
   -1.030200000000 /* ( 3, 0) */,
    1.288400000000 /* ( 3, 1) */,
   -1.464600000000 /* ( 3, 2) */,
    5.010200000000 /* ( 3, 3) */};

