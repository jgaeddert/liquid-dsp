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
// autotest fft data for 3-point transform
//

#include <complex.h>

float complex fft_test_x3[] = {
   -0.757661328095 +   0.655250200055*_Complex_I,
   -1.262964116539 +   0.804288531547*_Complex_I,
    0.075727215016 +  -0.825503865060*_Complex_I};

float complex fft_test_y3[] = {
   -1.944898229617 +   0.634034866542*_Complex_I,
    1.247398741022 +   1.825198567765*_Complex_I,
   -1.575484495691 +  -0.493482834141*_Complex_I};

