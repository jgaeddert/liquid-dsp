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
// autotest fft data for 9-point transform
//

#include <complex.h>

float complex fft_test_x9[] = {
    0.318149471742 +  -0.872622265472*_Complex_I,
    0.380460329361 +   0.204662364547*_Complex_I,
   -0.569767779072 +  -0.271995206036*_Complex_I,
    1.334787120105 +  -0.238015105170*_Complex_I,
   -0.644864052383 +   0.948536285238*_Complex_I,
   -0.489784794370 +   0.158143326416*_Complex_I,
    1.783096398872 +  -2.166235062454*_Complex_I,
   -0.138901921376 +  -0.646377338691*_Complex_I,
   -0.257444231274 +   0.857372365765*_Complex_I};

float complex fft_test_y9[] = {
    1.715730541604 +  -2.026530635857*_Complex_I,
    1.685963762512 +   0.399226582084*_Complex_I,
   -4.115380429157 +   1.255898079784*_Complex_I,
    4.091196716626 +  -4.693323087763*_Complex_I,
   -1.668677608930 +  -1.439432143007*_Complex_I,
   -2.523905986916 +  -0.920217192051*_Complex_I,
    4.501171713926 +  -3.110763575667*_Complex_I,
    1.269999384456 +   3.193455688437*_Complex_I,
   -2.092752848444 +  -0.511914105207*_Complex_I};

