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
// Filter API: complex floating-point
//

#include "liquid.internal.h"

// naming extensions (useful for print statements)
#define EXTENSION_SHORT     "f"
#define EXTENSION_FULL      "crcf"

// 
#define AUTOCORR(name)      LIQUID_CONCAT(autocorr_crcf,name)
#define FFTFILT(name)       LIQUID_CONCAT(fftfilt_crcf,name)
#define FIRDECIM(name)      LIQUID_CONCAT(firdecim_crcf,name)
#define FIRFARROW(name)     LIQUID_CONCAT(firfarrow_crcf,name)
#define FIRFILT(name)       LIQUID_CONCAT(firfilt_crcf,name)
#define FIRINTERP(name)     LIQUID_CONCAT(firinterp_crcf,name)
#define FIRPFB(name)        LIQUID_CONCAT(firpfb_crcf,name)
#define IIRDECIM(name)      LIQUID_CONCAT(iirdecim_crcf,name)
#define IIRFILT(name)       LIQUID_CONCAT(iirfilt_crcf,name)
#define IIRFILTSOS(name)    LIQUID_CONCAT(iirfiltsos_crcf,name)
#define IIRINTERP(name)     LIQUID_CONCAT(iirinterp_crcf,name)
#define MSRESAMP(name)      LIQUID_CONCAT(msresamp_crcf,name)
#define MSRESAMP2(name)     LIQUID_CONCAT(msresamp2_crcf,name)
#define RESAMP(name)        LIQUID_CONCAT(resamp_crcf,name)
#define RESAMP2(name)       LIQUID_CONCAT(resamp2_crcf,name)
#define SYMSYNC(name)       LIQUID_CONCAT(symsync_crcf,name)

#define T                   float complex   // general
#define TO                  float complex   // output
#define TC                  float           // coefficients
#define TI                  float complex   // input
#define WINDOW(name)        LIQUID_CONCAT(windowcf,name)
#define DOTPROD(name)       LIQUID_CONCAT(dotprod_crcf,name)
#define POLY(name)          LIQUID_CONCAT(polyf,name)

#define TO_COMPLEX          1
#define TC_COMPLEX          0
#define TI_COMPLEX          1

#define PRINTVAL_TO(X,F)    PRINTVAL_CFLOAT(X,F)
#define PRINTVAL_TC(X,F)    PRINTVAL_FLOAT(X,F)
#define PRINTVAL_TI(X,F)    PRINTVAL_CFLOAT(X,F)

// source files
//#include "autocorr.c"
#include "fftfilt.c"
#include "firdecim.c"
#include "firfarrow.c"
#include "firfilt.c"
#include "firinterp.c"
#include "firpfb.c"
#include "iirdecim.c"
#include "iirfilt.c"
#include "iirfiltsos.c"
#include "iirinterp.c"
#include "msresamp.c"
#include "msresamp2.c"
#include "resamp.c"         // floating-point phase version
//#include "resamp.fixed.c" // fixed-point phase version
#include "resamp2.c"
#include "symsync.c"
