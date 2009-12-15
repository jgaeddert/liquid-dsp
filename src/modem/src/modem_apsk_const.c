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

// APSK32(4,12)
const unsigned int apsk16_num_levels = 2;
const unsigned int apsk16_p[2] = {4,12};

const float apsk16_r[2] = {
    0.43246540f,
    1.12738252f};

const float apsk16_phi[2] = {
    0.0f,
    0.0f};

const float apsk16_r_slicer[1] = {
    0.77992396f};

const unsigned int apsk16_symbol_map[16] = {
    11,  10,   8,   9,  12,   2,   7,   1,
    14,  15,   5,   4,  13,   3,   6,   0};


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
  26,  25,  22,  23,  27,  11,  21,   9,
  13,   3,   7,   1,  12,  10,   8,  24,
  30,  31,  18,  17,  29,  15,  19,   5,
  28,   0,  20,   2,  14,  16,   6,   4};


