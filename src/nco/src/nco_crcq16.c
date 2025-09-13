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
// numerically-controlled oscillator (nco) API, floating point precision
//

#include "liquid.internal.h"

// declare fixed-point name-mangling macros
#define LIQUID_FIXED
#define Q(name)                 LIQUID_CONCAT(q16,name)
#define CQ(name)                LIQUID_CONCAT(cq16,name)

// data types
#define T                       q16_t   // primitive general
#define TC                      cq16_t  // coefficients

// naming extensions (useful for print statements)
#define EXTENSION               "crcq16"

// macros
#define NCO(name)               LIQUID_CONCAT(nco_crcq16,name)
//#define SYNTH(name)             LIQUID_CONCAT(synth_crcq16,name)
//#define IIRFILT_RRR(name)       LIQUID_CONCAT(iirfilt_rrrq16,   name)
//#define IIRFILTSOS_RRR(name)    LIQUID_CONCAT(iirfiltsos_rrrq16,name)

// supporting objects/methods
#define SIN                     q16_sin
#define COS                     q16_cos
#define SQRT                    q16_sqrt
#define CONJ                    cq16_conj

// constants, etc.
//#define NCO_ONE                 (1.0f)
//#define NCO_PI                  (M_PI)
//#define NCO_2PI                 (2.0f*M_PI)

// literal macros
#define TIL_(l)                 l ## .0f
#define TFL_(l)                 l ## f
#define TIL(l)                  TIL_(l)
#define TFL(l)                  TFL_(l)

// prototypes
#include "nco.proto.c"
//#include "synth.proto.c"

