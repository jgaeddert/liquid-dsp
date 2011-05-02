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

// APSK4(1,3)
const unsigned int apsk4_num_levels = 2;
const unsigned int apsk4_p[2] = {1,3};

const float apsk4_r[2] = {
    0.00000000,  1.15470052};

const float apsk4_phi[2] = {
    0.00000000,  0.00000000};

const float apsk4_r_slicer[1] = {
    0.57735026};

const unsigned int apsk4_symbol_map[4] = {
    3,2,1,0};
  

// APSK8(1,7)
const unsigned int apsk8_num_levels = 2;
const unsigned int apsk8_p[2] = {1,7};

const float apsk8_r[2] = {
    0.00000000,  1.06904495};

const float apsk8_phi[2] = {
    0.00000000,  0.00000000};

const float apsk8_r_slicer[1] = {
    0.53452247};

const unsigned int apsk8_symbol_map[8] = {
     0,   2,   4,   3,   1,   7,   5,   6};
  
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

#if 0
// APSK64(4,12,16,32)
const unsigned int apsk64_num_levels = 4;
const unsigned int apsk64_p[4] = {
  4,12,16,32};

const float apsk64_r[4] = {
    0.19348209,  0.50553697,  0.80761099,  1.25433850};

const float apsk64_phi[4] = {
    0.00000000, -0.00880714, -0.08513311, -0.04266101};

const float apsk64_r_slicer[3] = {
    0.34950953,  0.65657398,  1.03097475};

const unsigned int apsk64_symbol_map[64] = {
    52,  51,  49,  50,  44,  45,  47,  46,
    53,  26,  48,  25,  43,  22,  24,  23,
    55,  28,   4,  12,  41,   7,  32,   8,
    54,  27,  10,  11,  42,  21,   2,   9,
    60,  61,  63,  62,  36,  35,  33,  34,
    59,  30,  16,  31,  37,  18,  17,   5,
    57,  56,   0,   3,  40,  20,   1,   6,
    58,  29,  13,  14,  38,  19,  15,  39};
#else
// APSK64(4,14,20,26)
const unsigned int apsk64_num_levels = 4;
const unsigned int apsk64_p[4] = {
  4,14,20,26};

const float apsk64_r[4] = {
    0.18916586,  0.52466476,  0.88613129,  1.30529201};

const float apsk64_phi[4] = {
    0.0f, 0.0f, 0.0f, 0.0f};

const float apsk64_r_slicer[3] = {
    0.35691531,  0.70539802,  1.09571165};

const unsigned int apsk64_symbol_map[64] = {
    54,  53,  51,  52,  48,  49,  28,  50,
    55,  30,  11,  29,  47,  25,  27,  26,
    57,  32,   2,  14,  45,  23,   1,   8,
    56,  31,  12,  13,  46,  24,  10,   9,
    61,  62,  38,  63,  41,  40,  18,  39,
    60,  35,  37,  36,  42,  20,   4,  19,
    58,  33,   3,  15,  44,  22,   0,   7,
    59,  34,  17,  16,  43,  21,   5,   6};
  
#endif


// APSK128(8,18,24,36,42)
const unsigned int apsk128_num_levels = 5;
const unsigned int apsk128_p[5] = {8,18,24,36,42};
const float apsk128_r[5] = {
    0.20241030,  0.46255755,  0.70972824,  0.99172282,  1.34806108};

const float apsk128_phi[5] = {
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

const float apsk128_r_slicer[4] = {
    0.33248392,  0.58614290,  0.85072553,  1.16989195};

const unsigned int apsk128_symbol_map[128] = {
   112, 111, 108, 109, 102, 103, 106, 105,
   113, 110, 107,  71, 101, 104,  67,  66,
   115,  73,  39,  41,  99,  63,  38,  36,
   114,  72,  69,  70, 100,  64,  68,  65,
   117,  77,   1,  21,  97,  59,   2,  13,
    76,  43,   4,  20,  60,  33,   3,  14,
   116,  74,  18,  40,  98,  62,  37,  35,
    75,  42,  17,  19,  61,  34,  16,  15,
   123, 124, 127, 126,  91,  90,  87,  88,
   122, 125,  85,  84,  92,  89,  51,  53,
   120,  81,  49,  48,  94,  55,  26,  29,
   121,  82,  50,  83,  93,  54,  27,  52,
   118,  44,   7,   5,  96,  32,   0,  10,
    78,  45,   6,  22,  58,  30,  86,  12,
    80,  79,  25,  47,  57,  56,   9,  28,
   119,  46,  24,  23,  95,  31,   8,  11};
  
// APSK256(6,18,32,36,46,54,64)
const unsigned int apsk256_num_levels = 7;
const unsigned int apsk256_p[7] = {6,18,32,36,46,54,64};
const float apsk256_r[7] = {
    0.19219166,  0.41951191,  0.60772800,  0.77572918,  0.94819963,  1.12150347,  1.31012368};

const float apsk256_phi[7] = {
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

const float apsk256_r_slicer[6] = {
    0.30585179,  0.51361996,  0.69172859,  0.86196440,  1.03485155,  1.21581364};

const unsigned int apsk256_symbol_map[256] = {
   232, 231, 229, 230, 224, 225, 227, 226,
   216, 217, 219, 218, 164, 223, 221, 222,
   233, 228, 170, 171, 166, 167, 169, 168,
   215, 220, 160, 159, 165, 163, 161, 162,
   235, 173, 123, 121,  75,  76,  78,  77,
   213, 157,  70, 109,  73, 115,  71,  72,
   234, 172, 120, 174, 116, 117, 119, 118,
   214, 158, 110, 156, 114, 113, 111, 112,
   239, 178,  82, 126, 255,  19,  47,   5,
   209, 152,  66, 105,  56,  12,  33,  13,
   238, 177,  81, 125, 138, 193,  46,   4,
   210, 153,  67, 106,   7,   1,  34,   2,
   236, 175,  79, 122,  74,  42,  44,  43,
   212, 155,  69, 108,  39,  38,  36,  37,
   237, 176,  80, 124,   3,  40,  45,  41,
   211, 154,  68, 107,  16,  15,  35,  14,
   248, 249, 251, 250, 191, 190, 252, 253,
   200, 199, 197, 198, 139, 140, 196, 195,
   247, 185, 187, 186, 137, 136, 188, 189,
   201, 145, 143, 144,  93,  94, 142, 141,
   245, 183,  87, 131,  54,  53,  88,  89,
   203, 147,  61,  99,  26,  27,  60,  59,
   246, 184, 133, 132,  91,  90, 134, 135,
   202, 146,  97,  98,  57,  58,  96,  95,
   240, 179,  83, 127,  23, 192,  48,  17,
   208, 151,  65, 104,   6,  11,  32, 206,
   241, 180,  84, 128, 254, 242,  49,  18,
   207, 150,  64, 102,  92, 194,  31,  10,
   244, 182,  86, 130,  55,  21,  52,  51,
   204, 148,  62, 100,  25,   8,  28,  29,
   243, 181,  85, 129,   0,  22,  50,  20,
   205, 149,  63, 101,  24,   9,  30, 103};
  
