/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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
// modem_apsk_const.c
//
// Constant APSK modems
//

#include <stdlib.h>
#include "liquid.internal.h"

// APSK32(4,12,16)
const unsigned int apsk32_num_levels = 3;
const unsigned int apsk32_p[3] = {4,12,16};

const float apsk32_r[3] = {
    0.27952856f,
    0.72980529f,
    1.25737989f};

const float apsk32_phi[3] = {
    0.0f,
    0.0f,
    0.0f};

const float apsk32_r_slicer[2] = {
    0.504666925,
    0.993592590};

const unsigned int apsk32_symbol_map[32] = {
    0,1,2,3,
    4,5,6,7,8,9,10,11,12,13,14,15,
    16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};

