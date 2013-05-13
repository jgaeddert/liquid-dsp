/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2013 Joseph Gaeddert
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
// numerically-controlled oscillator (nco) API, 16-bit fixed-point precision
//

#include "liquid.internal.h"

#define LIQUID_FPM              (1)
#define NCO(name)               LIQUID_CONCAT(nco_crcq16,name)
#define T                       q16_t
#define TC                      cq16_t
#define Q(name)                 LIQUID_CONCAT(q16,name)
#define CQ(name)                LIQUID_CONCAT(cq16,name)

// supporting objects/methods
#define SIN                     q16_sin
#define COS                     q16_cos
#define IIRFILT_RRR(name)       LIQUID_CONCAT(iirfilt_rrrq16,   name)
#define IIRFILTSOS_RRR(name)    LIQUID_CONCAT(iirfiltsos_rrrq16,name)

// constants, etc.
#define NCO_ONE                 q16_one
#define NCO_PI                  q16_pi
#define NCO_2PI                 q16_2pi

// include main source file
#include "nco.c"

