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
// Filter API: floating-point
//

#include "liquid.internal.h"

// naming extensions (useful for print statements)
#define EXTENSION_SHORT     "f"
#define EXTENSION_FULL      "rrrf"

// 
#define AUTOCORR(name)      LIQUID_CONCAT(autocorr_rrrf,name)
#define FFTFILT(name)       LIQUID_CONCAT(fftfilt_rrrf,name)
#define FIRDECIM(name)      LIQUID_CONCAT(firdecim_rrrf,name)
#define FIRFARROW(name)     LIQUID_CONCAT(firfarrow_rrrf,name)
#define FIRFILT(name)       LIQUID_CONCAT(firfilt_rrrf,name)
#define FIRINTERP(name)     LIQUID_CONCAT(firinterp_rrrf,name)
#define FIRHILB(name)       LIQUID_CONCAT(firhilbf,name)
#define FIRPFB(name)        LIQUID_CONCAT(firpfb_rrrf,name)
#define IIRDECIM(name)      LIQUID_CONCAT(iirdecim_rrrf,name)
#define IIRFILT(name)       LIQUID_CONCAT(iirfilt_rrrf,name)
#define IIRFILTSOS(name)    LIQUID_CONCAT(iirfiltsos_rrrf,name)
#define IIRINTERP(name)     LIQUID_CONCAT(iirinterp_rrrf,name)
#define MSRESAMP(name)      LIQUID_CONCAT(msresamp_rrrf,name)
#define MSRESAMP2(name)     LIQUID_CONCAT(msresamp2_rrrf,name)
#define RESAMP(name)        LIQUID_CONCAT(resamp_rrrf,name)
#define RESAMP2(name)       LIQUID_CONCAT(resamp2_rrrf,name)
#define SYMSYNC(name)       LIQUID_CONCAT(symsync_rrrf,name)

#define T                   float   // general
#define TO                  float   // output
#define TC                  float   // coefficients
#define TI                  float   // input
#define WINDOW(name)        LIQUID_CONCAT(windowf,name)
#define DOTPROD(name)       LIQUID_CONCAT(dotprod_rrrf,name)
#define POLY(name)          LIQUID_CONCAT(polyf,name)

#define TO_COMPLEX          0
#define TC_COMPLEX          0
#define TI_COMPLEX          0

#define PRINTVAL_TO(X,F)    PRINTVAL_FLOAT(X,F)
#define PRINTVAL_TC(X,F)    PRINTVAL_FLOAT(X,F)
#define PRINTVAL_TI(X,F)    PRINTVAL_FLOAT(X,F)

// source files
#include "autocorr.c"
#include "fftfilt.c"
#include "firdecim.c"
#include "firfarrow.c"
#include "firfilt.c"
#include "firinterp.c"
#include "firhilb.c"
#include "firpfb.c"
#include "iirdecim.c"
#include "iirfilt.c"
#include "iirfiltsos.c"
#include "iirinterp.c"
#include "msresamp.c"
#include "msresamp2.c"
#include "resamp.c"
#include "resamp2.c"
#include "symsync.c"
