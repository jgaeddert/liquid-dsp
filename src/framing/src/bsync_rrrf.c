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

// 
#define BSYNC(name)         LIQUID_CONCAT(bsync_rrrf,name)

#define PRINTVAL(x)         printf("%12.4e", x)

#define TO                  float
#define TC                  float
#define TI                  float
#define ABS(X)              fabsf(X)
#define WINDOW(name)        LIQUID_CONCAT(windowf,name)
#define DOTPROD(name)       LIQUID_CONCAT(dotprod_rrrf,name)

#undef  TO_COMPLEX
#undef  TC_COMPLEX
#undef  TI_COMPLEX

// source files
#include "bsync.c"

