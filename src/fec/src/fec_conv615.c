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
// 1/6-rate K=15 convolutional code
//

#include "liquid.internal.h"

#define FEC_CONV(name)      LIQUID_CONCAT(fec_conv615,name)
#define create_viterbi      create_viterbi615
#define init_viterbi        init_viterbi615
#define update_viterbi_blk  update_viterbi615_blk
#define chainback_viterbi   chainback_viterbi615
#define delete_viterbi      delete_viterbi615

#define fec_conv615_R        (6)
#define fec_conv615_K        (15)
#define fec_conv615_mode     FEC_CONV_V615

#if HAVE_FEC_H  // (config.h)
#  include "fec.h"
#else
#  define V615POLYA  (0x00)
#  define V615POLYB  (0x00)
#  define V615POLYC  (0x00)
#  define V615POLYD  (0x00)
#  define V615POLYE  (0x00)
#  define V615POLYF  (0x00)
#endif  // HAVE_FEC_H (config.h)

const int FEC_CONV(_poly)[FEC_CONV(_R)] = {V615POLYA,
                                           V615POLYB,
                                           V615POLYC,
                                           V615POLYD,
                                           V615POLYE,
                                           V615POLYF};

#include "fec_conv.macro.c"

