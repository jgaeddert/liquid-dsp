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
// 1/2-rate K=7 convolutional code
//

#include "liquid.internal.h"

#define FEC_CONV(name)      LIQUID_CONCAT(fec_conv27,name)
#define create_viterbi      create_viterbi27
#define init_viterbi        init_viterbi27
#define update_viterbi_blk  update_viterbi27_blk
#define chainback_viterbi   chainback_viterbi27
#define delete_viterbi      delete_viterbi27
#define R                   (2)
#define K                   (7)
#define fec_conv27_mode     FEC_CONV_V27

// tail bits = R*K = 14 (round up to 16 bits = 2 bytes)

#if HAVE_FEC_H  // (config.h)
#  include "fec.h"
#else
#  define V27POLYA  (0x00)
#  define V27POLYB  (0x00)
#endif  // HAVE_FEC_H (config.h)

const int convpoly[R] = {V27POLYA,V27POLYB};

#include "fec_conv.macro.c"

