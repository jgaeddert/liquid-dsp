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
// Fixed-point math API: 16-bit q16 type
//

#include "liquidfpm.internal.h"

#define Q(name)     LIQUIDFPM_CONCAT(q16,name)
#define CQ(name)    LIQUIDFPM_CONCAT(cq16,name)

// source files
#include "qtype_arithmetic.port.c"
#include "qtype_conversion.c"
#include "qtype_inv_newton.c"
#include "qtype_log2_frac.c"
#include "qtype_exp2_frac.c"
#include "qtype_log2_shiftadd.c"
#include "qtype_exp2_shiftadd.c"
#include "qtype_atan_frac.c"
#include "qtype_atan_cordic.c"
#include "qtype_sincos_cordic.c"
#include "qtype_sincos_tab.c"
#include "qtype_sinhcosh_cordic.c"
#include "qtype_hyperbolic_shiftadd.c"
#include "qtype_sqrt.c"
#include "qtype_cbrt.c"

// specific type definitions (e.g. use cordic)
#include "qtype_trig.c"
#include "qtype_hyperbolic.c"
#include "qtype_logexp.c"
#include "qtype_power.c"
#include "qtype_erf.c"
#include "qtype_gamma.c"

// complex
#include "cqtype_arithmetic.c"
#include "cqtype_conversion.c"
#include "cqtype_logexp_cordic.c"
#include "cqtype_trig_cordic.c"
