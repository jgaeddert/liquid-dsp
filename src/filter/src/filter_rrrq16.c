/*
 * Copyright (c) 2007 - 2025 Joseph Gaeddert
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

//
// Filter API: fixed point
//

#include "liquid.internal.h"

// declare fixed-point name-mangling macros
#define LIQUID_FIXED
#define Q(name)             LIQUID_CONCAT(q16,name)
#define CQ(name)            LIQUID_CONCAT(cq16,name)

// naming extensions (useful for print statements)
#define EXTENSION_SHORT     "q16"
#define EXTENSION_FULL      "rrrq16"

// macros
#define FIRFILT(name)       LIQUID_CONCAT(firfilt_rrrq16,name)
#define FIRINTERP(name)     LIQUID_CONCAT(firinterp_rrrq16,name)
#define FIRPFB(name)        LIQUID_CONCAT(firpfb_rrrq16,name)
#define IIRFILT(name)       LIQUID_CONCAT(iirfilt_rrrq16,name)
#define IIRFILTSOS(name)    LIQUID_CONCAT(iirfiltsos_rrrq16,name)
#define IIRINTERP(name)     LIQUID_CONCAT(iirinterp_rrrq16,name)
#define RESAMP(name)        LIQUID_CONCAT(resamp_rrrq16,name)
#define RESAMP2(name)       LIQUID_CONCAT(resamp2_rrrq16,name)

#if 0
#define AUTOCORR(name)      LIQUID_CONCAT(autocorr_rrrq16,name)
#define FDELAY(name)        LIQUID_CONCAT(fdelay_rrrq16,name)
#define FFTFILT(name)       LIQUID_CONCAT(fftfilt_rrrq16,name)
#define FIRDECIM(name)      LIQUID_CONCAT(firdecim_rrrq16,name)
#define FIRFARROW(name)     LIQUID_CONCAT(firfarrow_rrrq16,name)
#define FIRHILB(name)       LIQUID_CONCAT(firhilbq16,name)
#define IIRDECIM(name)      LIQUID_CONCAT(iirdecim_rrrq16,name)
#define IIRHILB(name)       LIQUID_CONCAT(iirhilbq16,name)
#define MSRESAMP(name)      LIQUID_CONCAT(msresamp_rrrq16,name)
#define MSRESAMP2(name)     LIQUID_CONCAT(msresamp2_rrrq16,name)
#define ORDFILT(name)       LIQUID_CONCAT(ordfilt_rrrq16,name)
#define RRESAMP(name)       LIQUID_CONCAT(rresamp_rrrq16,name)
#define SYMSYNC(name)       LIQUID_CONCAT(symsync_rrrq16,name)
#endif

#define T                   q16_t   // general
#define TO                  q16_t   // output
#define TC                  q16_t   // coefficients
#define TI                  q16_t   // input
#define WINDOW(name)        LIQUID_CONCAT(windowq16,name)
#define DOTPROD(name)       LIQUID_CONCAT(dotprod_rrrq16,name)
//#define POLY(name)          LIQUID_CONCAT(polyq16,name)

// basic arithmetic operations
#define MUL_TI_TC(X,Y)      q16_mul(X,Y)
#define ADD_TO_TO(X,Y)      q16_add(X,Y)
#define SUB_TO_TO(X,Y)      q16_sub(X,Y)

// basic constants
#define TO_ZERO             (q16_zero)
#define TC_ONE              (q16_one)

#define TO_COMPLEX          0
#define TC_COMPLEX          0
#define TI_COMPLEX          0

#define PRINTVAL_TO(X,F)    PRINTVAL_Q16(X,F)
#define PRINTVAL_TC(X,F)    PRINTVAL_Q16(X,F)
#define PRINTVAL_TI(X,F)    PRINTVAL_Q16(X,F)

// prototype files
#include "firfilt.proto.c"
#include "firinterp.proto.c"
#include "firpfb.proto.c"
#include "iirfilt.proto.c"
#include "iirfiltsos.proto.c"
#include "iirinterp.proto.c"
//#include "resamp.proto.c"         // floating-point phase version
#include "resamp.fixed.proto.c" // fixed-point phase version
#include "resamp2.proto.c"

#if 0
#include "autocorr.proto.c"
#include "fdelay.proto.c"
#include "fftfilt.proto.c"
#include "firdecim.proto.c"
#include "firfarrow.proto.c"
#include "firhilb.proto.c"
#include "iirdecim.proto.c"
#include "iirhilb.proto.c"
#include "msresamp.proto.c"
#include "msresamp2.proto.c"
#include "ordfilt.proto.c"
#include "rresamp.proto.c"
#include "symsync.proto.c"
#endif
