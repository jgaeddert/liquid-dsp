/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
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
// Filter API: fixed point
//

#include "liquid.internal.h"

// declare fixed-point math
#define LIQUID_FIXED
#define  qtype_fixed_to_float(X)  q16_fixed_to_float(X)
#define  qtype_float_to_fixed(X)  q16_float_to_fixed(X)
#define cqtype_fixed_to_float(X) cq16_fixed_to_float(X)
#define cqtype_float_to_fixed(X) cq16_float_to_fixed(X)

// naming extensions (useful for print statements)
#define EXTENSION_SHORT     "q16"
#define EXTENSION_FULL      "rrrq16"

// 
#define FIRFILT(name)       LIQUID_CONCAT(firfilt_rrrq16,name)

#if 0
#define AUTOCORR(name)      LIQUID_CONCAT(autocorr_rrrq16,name)
#define FIRFARROW(name)     LIQUID_CONCAT(firfarrow_rrrq16,name)
#define FIRHILB(name)       LIQUID_CONCAT(firhilbf,name)
#define FIRPFB(name)        LIQUID_CONCAT(firpfb_rrrq16,name)
#define IIRFILT(name)       LIQUID_CONCAT(iirfilt_rrrq16,name)
#define IIRFILTSOS(name)    LIQUID_CONCAT(iirfiltsos_rrrq16,name)
#define INTERP(name)        LIQUID_CONCAT(interp_rrrq16,name)
#define DECIM(name)         LIQUID_CONCAT(decim_rrrq16,name)
#define MSRESAMP(name)      LIQUID_CONCAT(msresamp_rrrq16,name)
#define RESAMP(name)        LIQUID_CONCAT(resamp_rrrq16,name)
#define RESAMP2(name)       LIQUID_CONCAT(resamp2_rrrq16,name)
#define SYMSYNC(name)       LIQUID_CONCAT(symsync_rrrq16,name)
#endif

#define T                   q16_t   // general
#define TO                  q16_t   // output
#define TC                  q16_t   // coefficients
#define TI                  q16_t   // input
#define WINDOW(name)        LIQUID_CONCAT(windowq16,name)
#define DOTPROD(name)       LIQUID_CONCAT(dotprod_rrrq16,name)
//#define POLY(name)          LIQUID_CONCAT(polyq16,name)

#define TO_COMPLEX          0
#define TC_COMPLEX          0
#define TI_COMPLEX          0

#define PRINTVAL_TO(X,F)    PRINTVAL_Q16(X,F)
#define PRINTVAL_TC(X,F)    PRINTVAL_Q16(X,F)
#define PRINTVAL_TI(X,F)    PRINTVAL_Q16(X,F)

// source files
#include "firfilt.c"

#if 0
#include "autocorr.c"
#include "firfarrow.c"
#include "firhilb.c"
#include "firpfb.c"
#include "iirfilt.c"
#include "iirfiltsos.c"
#include "interp.c"
#include "decim.c"
#include "msresamp.c"
#include "resamp.c"
#include "resamp2.c"
#include "symsync.c"
#endif
