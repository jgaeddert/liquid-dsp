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
// autotest fft data for 5-point transform
//

#include <complex.h>

float complex fft_test_x5[] = {
    1.043452789296 +  -0.216675780077*_Complex_I,
   -0.039259154719 +  -0.756503590362*_Complex_I,
   -1.378329383804 +  -1.629692578129*_Complex_I,
    0.695728357044 +  -2.639675956000*_Complex_I,
   -0.019932891052 +   0.123958045411*_Complex_I};

float complex fft_test_y5[] = {
    0.301659716765 +  -5.118589859158*_Complex_I,
    1.333681830770 +   4.279329517647*_Complex_I,
   -0.597668794979 +  -2.985429553632*_Complex_I,
    2.358478480201 +   0.936943320049*_Complex_I,
    1.821112713724 +   1.804367674708*_Complex_I};

