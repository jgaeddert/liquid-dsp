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
// autotest fft data for 10-point transform
//

#include <complex.h>

float complex fft_test_x10[] = {
   -0.380648737020 +   1.003981780953*_Complex_I,
    1.031511152163 +  -2.625896014009*_Complex_I,
   -1.083239396623 +   1.646877001105*_Complex_I,
    0.951587457487 +  -0.004983138281*_Complex_I,
    0.407589360084 +   0.345698641918*_Complex_I,
    0.549291472049 +   0.542579734652*_Complex_I,
   -0.911825526748 +   1.282009726257*_Complex_I,
   -0.617849040964 +   0.696673367751*_Complex_I,
    1.097501043733 +   1.373947311009*_Complex_I,
    0.848713422957 +  -0.738252787172*_Complex_I};

float complex fft_test_y10[] = {
    1.892631207117 +   3.522635624182*_Complex_I,
   -1.167216826866 +  -3.158947047615*_Complex_I,
   -0.019614668329 +   1.291770408491*_Complex_I,
   -3.842057814631 +  -1.668342848977*_Complex_I,
   -2.323082893679 +   1.200058008683*_Complex_I,
   -3.633877720265 +   7.782393298301*_Complex_I,
    0.649048218143 +   6.997144832999*_Complex_I,
    1.025423884758 +   1.907666229150*_Complex_I,
    0.644231811894 +  -5.278801296330*_Complex_I,
    2.968027431661 +  -2.555759399357*_Complex_I};

