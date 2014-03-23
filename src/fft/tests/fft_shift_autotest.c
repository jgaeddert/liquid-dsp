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

#include "autotest/autotest.h"
#include "liquid.h"

void autotest_fft_shift_4()
{
    float complex x[] = {
        0 + 0*_Complex_I,
        1 + 1*_Complex_I,
        2 + 2*_Complex_I,
        3 + 3*_Complex_I
    };

    float complex test[] = {
        2 + 2*_Complex_I,
        3 + 3*_Complex_I,
        0 + 0*_Complex_I,
        1 + 1*_Complex_I
    };

    fft_shift(x,4);

    CONTEND_SAME_DATA(x,test,4*sizeof(float complex));
}

void autotest_fft_shift_8()
{
    float complex x[] = {
        0 + 0*_Complex_I,
        1 + 1*_Complex_I,
        2 + 2*_Complex_I,
        3 + 3*_Complex_I,
        4 + 4*_Complex_I,
        5 + 5*_Complex_I,
        6 + 6*_Complex_I,
        7 + 7*_Complex_I
    };

    float complex test[] = {
        4 + 4*_Complex_I,
        5 + 5*_Complex_I,
        6 + 6*_Complex_I,
        7 + 7*_Complex_I,
        0 + 0*_Complex_I,
        1 + 1*_Complex_I,
        2 + 2*_Complex_I,
        3 + 3*_Complex_I
    };

    fft_shift(x,8);

    CONTEND_SAME_DATA(x,test,8*sizeof(float complex));
}

