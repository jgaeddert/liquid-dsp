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
// autotest fft data for 4-point transform
//

#include <complex.h>

float complex fft_test_x4[] = {
   -2.218920151449 +  -1.079004048069*_Complex_I,
    0.045264423484 +   0.426155393025*_Complex_I,
    0.218614474268 +  -0.334711618319*_Complex_I,
    2.182538230032 +   1.706944462070*_Complex_I};

float complex fft_test_y4[] = {
    0.227496976335 +   0.719384188708*_Complex_I,
   -3.718323694762 +   1.392981376798*_Complex_I,
   -4.228108330697 +  -3.546815521483*_Complex_I,
   -1.156745556672 +  -2.881566236299*_Complex_I};

