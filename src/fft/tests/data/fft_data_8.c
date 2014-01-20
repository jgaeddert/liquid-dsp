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
// autotest fft data for 8-point transform
//

#include <complex.h>

float complex fft_test_x8[] = {
    1.143832659273 +   0.058730029889*_Complex_I,
   -0.094390429919 +   0.229144161540*_Complex_I,
   -0.231936945111 +   0.250418514706*_Complex_I,
    0.180568135767 +  -0.869698396678*_Complex_I,
   -0.345282052584 +   1.176003338020*_Complex_I,
    0.544428216952 +  -0.610473584454*_Complex_I,
    0.928035714223 +   0.647778401795*_Complex_I,
    0.441211141066 +  -1.176622015089*_Complex_I};

float complex fft_test_y8[] = {
    2.566466439667 +  -0.294719550271*_Complex_I,
    1.635071437815 +   1.055386414782*_Complex_I,
    1.767442826430 +   0.508277941207*_Complex_I,
    2.964612333261 +  -2.017902163711*_Complex_I,
    0.422832311935 +   4.560580119089*_Complex_I,
    0.548438211721 +  -0.969987712376*_Complex_I,
   -1.562539151277 +   0.164794961607*_Complex_I,
    0.808336864628 +  -2.536589771219*_Complex_I};

