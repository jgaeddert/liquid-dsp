/*
 * Copyright (c) 2007, 2008, 2009, 2010, 2011, 2012, 2013 Joseph Gaeddert
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
// AGC API: fixed-point [complex]
//

#include "liquid.internal.h"
#include "liquidfpm.h"

// declare fixed-point name-mangling macros
#define LIQUID_FIXED
#define Q(name)             LIQUID_CONCAT(q16,name)
#define CQ(name)            LIQUID_CONCAT(cq16,name)

// naming extensions (useful for print statements)
#define EXTENSION_SHORT     "q16"
#define EXTENSION_FULL      "crcq16"

// macros
#define AGC(name)           LIQUID_CONCAT(agc_crcq16,name)

#define T                   q16_t           // general
#define TC                  cq16_t          // input/output

#define TC_COMPLEX          1

// source files
#include "agc.c"
