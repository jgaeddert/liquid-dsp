/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
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
// Filter API: complex floating-point
//

#include "liquid.internal.h"

// naming extensions (useful for print statements)
#define EXTENSION_SHORT     "f"
#define EXTENSION_FULL      "cccf"

// 
#define AUTOCORR(name)      LIQUID_CONCAT(autocorr_cccf,name)
#define FFTFILT(name)       LIQUID_CONCAT(fftfilt_cccf,name)
#define FIRDECIM(name)      LIQUID_CONCAT(firdecim_cccf,name)
#define FIRFILT(name)       LIQUID_CONCAT(firfilt_cccf,name)
#define FIRINTERP(name)     LIQUID_CONCAT(firinterp_cccf,name)
#define FIRPFB(name)        LIQUID_CONCAT(firpfb_cccf,name)
#define IIRDECIM(name)      LIQUID_CONCAT(iirdecim_cccf,name)
#define IIRFILT(name)       LIQUID_CONCAT(iirfilt_cccf,name)
#define IIRFILTSOS(name)    LIQUID_CONCAT(iirfiltsos_cccf,name)
#define IIRINTERP(name)     LIQUID_CONCAT(iirinterp_cccf,name)
#define NCO(name)           LIQUID_CONCAT(nco_crcf,name)
#define MSRESAMP(name)      LIQUID_CONCAT(msresamp_cccf,name)
#define MSRESAMP2(name)     LIQUID_CONCAT(msresamp2_cccf,name)
#define RESAMP(name)        LIQUID_CONCAT(resamp_cccf,name)
#define RESAMP2(name)       LIQUID_CONCAT(resamp2_cccf,name)
//#define SYMSYNC(name)       LIQUID_CONCAT(symsync_cccf,name)

#define T                   float complex   // general
#define TO                  float complex   // output
#define TC                  float complex   // coefficients
#define TI                  float complex   // input
#define WINDOW(name)        LIQUID_CONCAT(windowcf,name)
#define DOTPROD(name)       LIQUID_CONCAT(dotprod_cccf,name)
#define POLY(name)          LIQUID_CONCAT(polyf,name)

#define TO_COMPLEX          1
#define TC_COMPLEX          1
#define TI_COMPLEX          1

#define PRINTVAL_TO(X,F)    PRINTVAL_CFLOAT(X,F)
#define PRINTVAL_TC(X,F)    PRINTVAL_CFLOAT(X,F)
#define PRINTVAL_TI(X,F)    PRINTVAL_CFLOAT(X,F)

// source files
#include "autocorr.c"
#include "fftfilt.c"
#include "firdecim.c"
#include "firfilt.c"
#include "firinterp.c"
#include "firpfb.c"
#include "iirdecim.c"
#include "iirfilt.c"
#include "iirfiltsos.c"
#include "iirinterp.c"
//#include "qmfb.c"
#include "msresamp.c"
#include "msresamp2.c"
#include "resamp.c"
#include "resamp2.c"
//#include "symsync.c"
