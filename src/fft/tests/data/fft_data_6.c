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
// autotest fft data for 6-point transform
//

#include <complex.h>

float complex fft_test_x6[] = {
   -0.946868805918 +   0.048419613876*_Complex_I,
   -1.426556442325 +   1.356194807524*_Complex_I,
    0.262357323076 +   1.594616904796*_Complex_I,
   -1.032912520662 +   0.046391595464*_Complex_I,
   -0.271359734201 +  -2.390517158747*_Complex_I,
   -0.288151144041 +   0.071324517238*_Complex_I};

float complex fft_test_y6[] = {
   -3.703491324072 +   0.726430280150*_Complex_I,
    3.797148775593 +   1.637413185851*_Complex_I,
   -3.456423352393 +   1.227102112087*_Complex_I,
    1.791748889984 +  -2.221391560299*_Complex_I,
    1.220570696725 +  -1.669098764217*_Complex_I,
   -5.330766521347 +   0.590062429687*_Complex_I};

