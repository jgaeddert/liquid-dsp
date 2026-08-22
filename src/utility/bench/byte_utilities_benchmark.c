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
#include <stdlib.h>

LIQUID_BENCHMARK(count_ones, "liquid_count_ones over buffer", "utility")
{
    // allocate buffer of bytes and initialize
    unsigned int i, j;
    unsigned int   buf_len = 1024;
    unsigned int * buf     = (unsigned int *) malloc(buf_len*sizeof(unsigned int));
    for (i=0; i<buf_len; i++)
        buf[i] = i & 0xff;
    // start trials (4 iterations over buf_len bytes each; round down)
    unsigned int c = 0;
    unsigned long int n = num_iterations / (4 * buf_len);
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        c &= 0xffff;
        for (j=0; j<buf_len; j++)
            c += liquid_count_ones(buf[j]);
    }
    float extime = liquid_toc(timer);
    // clean allocated memory
    free(buf);
    return extime;
}

