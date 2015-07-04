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
// Binary pre-demod synchronizer
//

#include "liquid.internal.h"

// 
#define BPRESYNC(name)      LIQUID_CONCAT(bpresync_cccf,name)

// print and naming extensions
#define PRINTVAL(x)         printf("%12.4e + j%12.4e", crealf(x), cimagf(x))
#define EXTENSION_SHORT     "f"
#define EXTENSION_FULL      "cccf"

#define TO                  float complex
#define TC                  float complex
#define TI                  float complex

#define ABS(X)              cabsf(X)
#define REAL(X)             crealf(X)
#define IMAG(X)             cimagf(X)

#define BSYNC(name)         LIQUID_CONCAT(bsync_cccf,name)

#define TO_COMPLEX
#define TC_COMPLEX
#define TI_COMPLEX

// source files
#include "bpresync.c"

