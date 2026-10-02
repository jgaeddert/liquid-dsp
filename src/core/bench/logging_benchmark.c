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

// test overhead for basic logging
LIQUID_BENCHMARK(logging, "logging overhead (info-level no-op)", "core,logging")
{
    // use a private logger so the global logger (used by the harness for
    // per-benchmark output and the summary) is never disturbed
    liquid_logger log = liquid_logger_create();
    liquid_logger_set_level(log, LIQUID_WARN); // suppress INFO-level output

    // start trials (4 log calls per iteration; round down)
    unsigned long int i, n = num_iterations / 4;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        liquid_log(log, LIQUID_INFO, LIQUID_FILENAME, __LINE__, "log event %i:0", i);
        liquid_log(log, LIQUID_INFO, LIQUID_FILENAME, __LINE__, "log event %i:1", i);
        liquid_log(log, LIQUID_INFO, LIQUID_FILENAME, __LINE__, "log event %i:2", i);
        liquid_log(log, LIQUID_INFO, LIQUID_FILENAME, __LINE__, "log event %i:3", i);
    }
    float extime = liquid_toc(timer);
    liquid_logger_destroy(log);
    return extime;
}

