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
// autotest fft data for 7-point transform
//

#include <complex.h>

float complex fft_test_x7[] = {
    0.325737557343 +   0.347762560645*_Complex_I,
   -0.464568614672 +   1.344201995758*_Complex_I,
   -1.458140194879 +   0.983317270098*_Complex_I,
    1.679041515327 +   1.025013762005*_Complex_I,
   -0.178483024495 +  -0.711524629930*_Complex_I,
    0.986194459374 +  -1.709315563086*_Complex_I,
    0.387998802736 +  -1.150726066104*_Complex_I};

float complex fft_test_y7[] = {
    1.277780500734 +   0.128729329387*_Complex_I,
    4.360250363806 +   2.591163135631*_Complex_I,
    1.609972293897 +   2.377175130550*_Complex_I,
    0.436888889637 +  -3.701058823864*_Complex_I,
   -0.903757801309 +   3.003131513942*_Complex_I,
    1.797162255231 +  -0.068636624441*_Complex_I,
   -6.298133600593 +  -1.896165736688*_Complex_I};

