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

#include "liquid.benchmark.h"

LIQUID_BENCHMARK(quantize_adc, "quantize_adc", "quantization,quantizer")
{
    unsigned long int i;
    unsigned int q = 0;
    unsigned int num_bits=8;
    float x=-0.1f;
    // start trials (4 quantizations per iteration; round down)
    unsigned long int n = num_iterations / 4;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        q ^= quantize_adc(x,num_bits);
        q ^= quantize_adc(x,num_bits);
        q ^= quantize_adc(x,num_bits);
        q ^= quantize_adc(x,num_bits);
    }
    float extime = liquid_toc(timer);
    return extime;
}

LIQUID_BENCHMARK(quantize_dac, "quantize_dac", "quantization,quantizer")
{
    unsigned long int i;
    unsigned int q=0x0f;
    unsigned int num_bits=8;
    float x = 0;
    // start trials (4 de-quantizations per iteration; round down)
    unsigned long int n = num_iterations / 4;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        x += quantize_dac(q,num_bits);
        x += quantize_dac(q,num_bits);
        x += quantize_dac(q,num_bits);
        x += quantize_dac(q,num_bits);
    }
    float extime = liquid_toc(timer);
    return extime;
}

