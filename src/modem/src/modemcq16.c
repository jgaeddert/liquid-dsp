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
// modemf.c : linear modem API, floating-point (single precision)
//

#include <math.h>
#include "liquid.internal.h"

// declare fixed-point math
#define LIQUID_FIXED
#define Q(name)             LIQUID_CONCAT(q16,name)
#define CQ(name)            LIQUID_CONCAT(cq16,name)

#define CREAL(X)            cq16_real(X)
#define CIMAG(X)            cq16_imag(X)
#define CONJ(X)             cq16_conj(X)

// Macro definitions
#define MODEM(name)         LIQUID_CONCAT(modemcq16,name)
//#define CPFSKDEM(name)      LIQUID_CONCAT(cpfskdemq16,name)
//#define CPFSKMOD(name)      LIQUID_CONCAT(cpfskmodq16,name)
#define FREQMOD(name)       LIQUID_CONCAT(freqmodq16,name)
#define FREQDEM(name)       LIQUID_CONCAT(freqdemq16,name)

// supporting objects
#define FIRFILT_CRC(name)   LIQUID_CONCAT(firfilt_crcq16,name)
#define IIRFILT_RRR(name)   LIQUID_CONCAT(iirfilt_rrrq16,name)
#define IIRFILT_CRC(name)   LIQUID_CONCAT(iirfilt_crcq16,name)
#define NCO_CRC(name)       LIQUID_CONCAT(nco_crcq16,name)

#define T                   q16_t           // primitive type
#define TC                  cq16_t          // primitive type (complex)

// basic constants
#define T_ZERO              (cq16_zero)
#define T_ONE               (cq16_one)

// basic functions
//#define PRINTVAL_T(X,F)     PRINTVAL_FLOAT(X,F)
//#define PRINTVAL_TC(X,F)    PRINTVAL_CFLOAT(X,F)

// naming extension (useful for print statements)
#define EXTENSION           "cq16"

// common source must come first (object definition)
#include "modem_common.proto.c"

// generic modem specifications
#include "modem_psk.proto.c"
#include "modem_dpsk.proto.c"
#include "modem_ask.proto.c"
#include "modem_qam.proto.c"
#include "modem_apsk.proto.c"

// specific modems
#include "modem_bpsk.proto.c"
#include "modem_qpsk.proto.c"
#include "modem_ook.proto.c"
#include "modem_pi4dqpsk.proto.c"
#include "modem_sqam32.proto.c"
#include "modem_sqam128.proto.c"

// arbitrary modems
#include "modem_arb.proto.c"

// non-linear modems
//#include "cpfskdem.proto.c"
//#include "cpfskmod.proto.c"

// analog modems
#include "freqmod.proto.c"
#include "freqdem.proto.c"

