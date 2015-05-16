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
// Framing API: floating-point
//

#include "liquid.internal.h"

// naming extensions (useful for print statements)
#define EXTENSION_SHORT     "f"
#define EXTENSION_FULL      "cccf"

#define PRINTVAL(x)         printf("%12.4e + j%12.4e", crealf(x), cimagf(x))

#define T                   float
#define TO                  float complex
#define TC                  float complex
#define TI                  float complex
#define ABS(X)              cabsf(X)

// object references
#define SYMTRACK(name)      LIQUID_CONCAT(symtrack_cccf,name)
#define AGC(name)           LIQUID_CONCAT(agc_crcf,name)
#define SYMSYNC(name)       LIQUID_CONCAT(symsync_crcf,name)
#define EQLMS(name)         LIQUID_CONCAT(eqlms_cccf,name)
#define NCO(name)           LIQUID_CONCAT(nco_crcf,name)
#define MODEM(name)         LIQUID_CONCAT(modem,name)

#define TO_COMPLEX          1
#define TC_COMPLEX          1
#define TI_COMPLEX          1

// source files
#include "symtrack.c"

