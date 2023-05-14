/*
 * Copyright (c) 2007 - 2023 Joseph Gaeddert
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
// API: floating-point
//

#include "liquid.internal.h"

// naming extensions (useful for print statements)
#define EXTENSION           "cccf"

#define TO                  float complex   // output type
#define TC                  float complex   // coefficients type
#define TI                  float complex   // input type
#define T                   float           // primitive type

// object references
#define QDSYNC(name)        LIQUID_CONCAT(qdsync_cccf,name)
#define QDETECTOR(name)     LIQUID_CONCAT(qdetector_cccf,name)

// prototypes
#include "qdsync.proto.c"

