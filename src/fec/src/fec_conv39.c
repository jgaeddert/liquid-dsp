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
// 1/3-rate K=9 convolutional code
//

#include "liquid.internal.h"

#define FEC_CONV(name)      LIQUID_CONCAT(fec_conv39,name)
#define create_viterbi      create_viterbi39
#define init_viterbi        init_viterbi39
#define update_viterbi_blk  update_viterbi39_blk
#define chainback_viterbi   chainback_viterbi39
#define delete_viterbi      delete_viterbi39

#define fec_conv39_R        (3)
#define fec_conv39_K        (9)
#define fec_conv39_mode     FEC_CONV_V39

#if HAVE_FEC_H  // (config.h)
#  include "fec.h"
#else
#  define V39POLYA  (0x00)
#  define V39POLYB  (0x00)
#  define V39POLYC  (0x00)
#endif  // HAVE_FEC_H (config.h)

const int FEC_CONV(_poly)[FEC_CONV(_R)] = {V39POLYA,
                                           V39POLYB,
                                           V39POLYC};

#include "fec_conv.macro.c"

