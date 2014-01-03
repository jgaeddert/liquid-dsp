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
// data for testing Q/R decomposition
//

#include <complex.h>

// matrixf_data_qrdecomp_A [size: 4 x 4]
float matrixf_data_qrdecomp_A[] = {
    1.000000000000 /* ( 0, 0) */,
    2.000000000000 /* ( 0, 1) */,
    3.000000000000 /* ( 0, 2) */,
    4.000000000000 /* ( 0, 3) */,
    5.000000000000 /* ( 1, 0) */,
    5.000000000000 /* ( 1, 1) */,
    7.000000000000 /* ( 1, 2) */,
    8.000000000000 /* ( 1, 3) */,
    6.000000000000 /* ( 2, 0) */,
    4.000000000000 /* ( 2, 1) */,
    8.000000000000 /* ( 2, 2) */,
    7.000000000000 /* ( 2, 3) */,
    1.000000000000 /* ( 3, 0) */,
    0.000000000000 /* ( 3, 1) */,
    3.000000000000 /* ( 3, 2) */,
    1.000000000000 /* ( 3, 3) */};

// matrixf_data_qrdecomp_Q [size: 4 x 4]
float matrixf_data_qrdecomp_Q[] = {
    0.125988157670 /* ( 0, 0) */,
    0.617707763884 /* ( 0, 1) */,
    0.571886263590 /* ( 0, 2) */,
    0.524890659168 /* ( 0, 3) */,
    0.629940788349 /* ( 1, 0) */,
    0.494166211107 /* ( 1, 1) */,
   -0.137252703262 /* ( 1, 2) */,
   -0.583211843520 /* ( 1, 3) */,
    0.755928946018 /* ( 2, 0) */,
   -0.444749589997 /* ( 2, 1) */,
   -0.114377252718 /* ( 2, 2) */,
    0.466569474816 /* ( 2, 3) */,
    0.125988157670 /* ( 3, 0) */,
   -0.420041279441 /* ( 3, 1) */,
    0.800640769025 /* ( 3, 2) */,
   -0.408248290464 /* ( 3, 3) */};

// matrixf_data_qrdecomp_R [size: 4 x 4]
float matrixf_data_qrdecomp_R[] = {
    7.937253933194 /* ( 0, 0) */,
    6.425396041157 /* ( 0, 1) */,
   11.212946032607 /* ( 0, 2) */,
   10.960969717268 /* ( 0, 3) */,
    0.000000000000 /* ( 1, 0) */,
    1.927248223319 /* ( 1, 1) */,
    0.494166211107 /* ( 1, 2) */,
    2.890872334978 /* ( 1, 3) */,
    0.000000000000 /* ( 2, 0) */,
    0.000000000000 /* ( 2, 1) */,
    2.241794153271 /* ( 2, 2) */,
    1.189523428266 /* ( 2, 3) */,
    0.000000000000 /* ( 3, 0) */,
    0.000000000000 /* ( 3, 1) */,
    0.000000000000 /* ( 3, 2) */,
    0.291605921760 /* ( 3, 3) */};

