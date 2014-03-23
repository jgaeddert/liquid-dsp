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
// data for testing matrix inversion
//

#include <complex.h>

// matrixf_data_inv_x [size: 5 x 5]
float matrixf_data_inv_x[] = {
    0.145655393600 /* ( 0, 0) */,
   -2.292126655579 /* ( 0, 1) */,
    0.928358852863 /* ( 0, 2) */,
    0.995244622231 /* ( 0, 3) */,
   -0.719965457916 /* ( 0, 4) */,
    1.625229239464 /* ( 1, 0) */,
    1.179069876671 /* ( 1, 1) */,
    0.023814691231 /* ( 1, 2) */,
   -0.458529949188 /* ( 1, 3) */,
    0.870123147964 /* ( 1, 4) */,
    1.599076509476 /* ( 2, 0) */,
    1.012132167816 /* ( 2, 1) */,
    0.240342438221 /* ( 2, 2) */,
   -0.663878023624 /* ( 2, 3) */,
    1.523158550262 /* ( 2, 4) */,
    1.400263786316 /* ( 3, 0) */,
   -0.016515849158 /* ( 3, 1) */,
    0.525676131248 /* ( 3, 2) */,
   -0.526886940002 /* ( 3, 3) */,
   -0.605886101723 /* ( 3, 4) */,
   -0.291201651096 /* ( 4, 0) */,
    0.635409533978 /* ( 4, 1) */,
    0.016531571746 /* ( 4, 2) */,
    0.113017730415 /* ( 4, 3) */,
   -0.886025428772 /* ( 4, 4) */};

// matrixf_data_inv_y [size: 5 x 5]
float matrixf_data_inv_y[] = {
    0.123047731616 /* ( 0, 0) */,
    1.264793339850 /* ( 0, 1) */,
   -0.888020214878 /* ( 0, 2) */,
    0.146648698334 /* ( 0, 3) */,
   -0.484762774689 /* ( 0, 4) */,
    0.031615676756 /* ( 1, 0) */,
   -0.041217620573 /* ( 1, 1) */,
    0.486809371567 /* ( 1, 2) */,
   -0.307386761818 /* ( 1, 3) */,
    0.980900315396 /* ( 1, 4) */,
    0.456515830075 /* ( 2, 0) */,
   -2.168499777786 /* ( 2, 1) */,
    2.469455722213 /* ( 2, 2) */,
    0.010642598564 /* ( 2, 3) */,
    1.737407148356 /* ( 2, 4) */,
    0.690799395919 /* ( 3, 0) */,
    1.532809684521 /* ( 3, 1) */,
   -0.611813824735 /* ( 3, 2) */,
   -1.028413056396 /* ( 3, 3) */,
    0.595460566672 /* ( 3, 4) */,
    0.078865348162 /* ( 4, 0) */,
   -0.290188077617 /* ( 4, 1) */,
    0.609005594780 /* ( 4, 2) */,
   -0.399620351004 /* ( 4, 3) */,
   -0.157493442155 /* ( 4, 4) */};

