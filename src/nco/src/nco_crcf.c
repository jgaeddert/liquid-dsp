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

// data types
#define T                       float           // primitive/general
#define TC                      float complex   // input/output/complex

// naming extensions (useful for print statements)
#define EXTENSION               "crcf"
//#define EXTENSION_SHORT         "f"
//#define EXTENSION_FULL          "crcf"

// macros
#define NCO(name)               LIQUID_CONCAT(nco_crcf,name)
#define SYNTH(name)             LIQUID_CONCAT(synth_crcf,name)

// supporting objects/methods
#define SIN                     sinf
#define COS                     cosf
#define SQRT                    sqrtf
#define CONJ                    conjf

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
#include "synth.proto.c"

