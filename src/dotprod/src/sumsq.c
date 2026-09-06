/*
 * Copyright (c) 2007 - 2026 Joseph Gaeddert
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
// sumsq.c : sum of squares
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "liquid.internal.h"

// forward declaration of internal methods

// select runtime execution method
int liquid_sumsqf_runtime_detect(void);

// execution methods: always defined but only really implemented on
// specific architectures
float liquid_sumsqf_execute_port  (float * _v, unsigned int _n);
float liquid_sumsqf_execute_sse   (float * _v, unsigned int _n);
float liquid_sumsqf_execute_avx   (float * _v, unsigned int _n);
float liquid_sumsqf_execute_avx512(float * _v, unsigned int _n);

// currently selected runtime execution method
static float (*liquid_sumsqf_execute)(float *, unsigned int) = NULL;

// sum squares, basic loop
//  _v      :   input array [size: 1 x _n]
//  _n      :   input length
float liquid_sumsqf_execute_port(float *      _v,
                                 unsigned int _n)
{
    // initialize accumulator
    float r=0;

    // t = 4*(floor(_n/4))
    unsigned int t=(_n>>2)<<2;

    // run computation in groups of 4
    unsigned int i;
    for (i=0; i<t; i+=4) {
        r += _v[i  ] * _v[i  ];
        r += _v[i+1] * _v[i+1];
        r += _v[i+2] * _v[i+2];
        r += _v[i+3] * _v[i+3];
    }

    // clean up remaining
    for ( ; i<_n; i++)
        r += _v[i] * _v[i];

    // return result
    return r;
}

// select runtime execution method
int liquid_sumsqf_runtime_select(liquid_runtime_t _select)
{
    switch (_select) {
    case LIQUID_RUNTIME_PORT:
        liquid_log_trace("liquid_sumsqf_runtime_select(), port");
        liquid_sumsqf_execute = &liquid_sumsqf_execute_port;
        return LIQUID_OK;
    case LIQUID_RUNTIME_SSE:
        liquid_log_trace("liquid_sumsqf_runtime_select(), sse");
        liquid_sumsqf_execute = &liquid_sumsqf_execute_sse;
        return LIQUID_OK;
    case LIQUID_RUNTIME_AVX:
        liquid_log_trace("liquid_sumsqf_runtime_select(), avx");
        liquid_sumsqf_execute = &liquid_sumsqf_execute_avx;
        return LIQUID_OK;
    case LIQUID_RUNTIME_AVX512:
        liquid_log_trace("liquid_sumsqf_runtime_select(), avx512");
        liquid_sumsqf_execute = &liquid_sumsqf_execute_avx512;
        return LIQUID_OK;
    default:;
    }

    liquid_sumsqf_execute = &liquid_sumsqf_execute_port;
    return liquid_error(LIQUID_EINT,
        "liquid_sumsqf_runtime_select(), invalid selection or mode not available (%d), falling back to portable version",
        _select);
}

// detect and select runtime execution method
int liquid_sumsqf_runtime_detect(void)
{
    // implementations currently available for this algorithm
    struct liquid_cpuinfo_s impl =
    {
        .altivec = 0,
        .neon    = 0,
        .mmx     = 0,
        .sse     = true,
        .sse2    = 0,
        .sse3    = 0,
        .ssse3   = 0,
        .sse41   = 0,
        .sse42   = 0,
        .avx     = true,
        .fma3    = 0,
        .avx2    = 0,
        .avx512  = true,
        .amx     = 0,
        .amx101  = 0,
        .amx102  = 0,
    };

    // given implementations, find best method
    liquid_runtime_t selection = liquid_runtime_detect(&impl);

    // invoke selection method
    return liquid_sumsqf_runtime_select(selection);
}

// sum squares
//  _v      :   input array [size: 1 x _n]
//  _n      :   input length
float liquid_sumsqf(float *      _v,
                    unsigned int _n)
{
    // select runtime method on first call
    if (liquid_sumsqf_execute == NULL)
        liquid_sumsqf_runtime_detect();

    return liquid_sumsqf_execute(_v, _n);
}

// sum squares, complex
//  _v      :   input array [size: 1 x _n]
//  _n      :   input length
float liquid_sumsqcf(float complex * _v,
                     unsigned int    _n)
{
    // simple method: type cast input as real pointer, run double
    // length sumsqf method
    float * v = (float*) _v;
    return liquid_sumsqf(v, 2*_n);
}

// SIMD extensions
#include "sumsq.sse.c"
#include "sumsq.avx.c"
#include "sumsq.avx512f.c"
