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
// fixed-point buffer
//

#include "liquid.internal.h"

// naming extensions (useful for print statements)
#define EXTENSION       "cq16"

#define CBUFFER(name)   LIQUID_CONCAT(cbuffercq16, name)
//#define WDELAY(name)    LIQUID_CONCAT(wdelaycq16, name)
#define WINDOW(name)    LIQUID_CONCAT(windowcq16, name)

#define T cq16_t
#define BUFFER_PRINT_LINE(B,I)  printf("    : %12.8f + j%12.8f", q16_fixed_to_float(B->v[I].real), q16_fixed_to_float(B->v[I].imag));
#define BUFFER_PRINT_VALUE(V)   printf("    : %12.4f + j%12.8f", q16_fixed_to_float(V.real), q16_fixed_to_float(V.imag));

#include "cbuffer.c"
//#include "wdelay.c"
#include "window.c"

