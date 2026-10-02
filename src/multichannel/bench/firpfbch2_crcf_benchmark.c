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

// Helper function to keep code base small
float firpfbch2_crcf_execute_bench(unsigned long int _num_iterations,
                                   unsigned int      _num_channels,
                                   unsigned int      _m,
                                   int               _type)
{
    // initialize channelizer
    float As         = 60.0f;
    firpfbch2_crcf q = firpfbch2_crcf_create_kaiser(_type,_num_channels,_m,As);
    unsigned long int i;
    float complex x[_num_channels];
    float complex y[_num_channels];
    for (i=0; i<_num_channels; i++)
        x[i] = 1.0f + _Complex_I*1.0f;
    // start trials (4 executes per iteration; round down)
    unsigned long int n = _num_iterations / 4;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        firpfbch2_crcf_execute(q, x, y);
        firpfbch2_crcf_execute(q, x, y);
        firpfbch2_crcf_execute(q, x, y);
        firpfbch2_crcf_execute(q, x, y);
    }
    float extime = liquid_toc(timer);
    firpfbch2_crcf_destroy(q);
    return extime;
}

// analysis
LIQUID_BENCHMARK(firpfbch2_crcf_a4,    "firpfbch2_crcf analyzer, M=4",    "multichannel,firpfbch2,analyzer")
    { return firpfbch2_crcf_execute_bench(num_iterations, 4,    2, LIQUID_ANALYZER); }
LIQUID_BENCHMARK(firpfbch2_crcf_a16,   "firpfbch2_crcf analyzer, M=16",   "multichannel,firpfbch2,analyzer")
    { return firpfbch2_crcf_execute_bench(num_iterations, 16,   2, LIQUID_ANALYZER); }
LIQUID_BENCHMARK(firpfbch2_crcf_a64,   "firpfbch2_crcf analyzer, M=64",   "multichannel,firpfbch2,analyzer")
    { return firpfbch2_crcf_execute_bench(num_iterations, 64,   2, LIQUID_ANALYZER); }
LIQUID_BENCHMARK(firpfbch2_crcf_a256,  "firpfbch2_crcf analyzer, M=256",  "multichannel,firpfbch2,analyzer")
    { return firpfbch2_crcf_execute_bench(num_iterations, 256,  2, LIQUID_ANALYZER); }
LIQUID_BENCHMARK(firpfbch2_crcf_a512,  "firpfbch2_crcf analyzer, M=512",  "multichannel,firpfbch2,analyzer")
    { return firpfbch2_crcf_execute_bench(num_iterations, 512,  2, LIQUID_ANALYZER); }
LIQUID_BENCHMARK(firpfbch2_crcf_a1024, "firpfbch2_crcf analyzer, M=1024", "multichannel,firpfbch2,analyzer")
    { return firpfbch2_crcf_execute_bench(num_iterations, 1024, 2, LIQUID_ANALYZER); }

// synthesis
LIQUID_BENCHMARK(firpfbch2_crcf_s4,    "firpfbch2_crcf synthesizer, M=4",    "multichannel,firpfbch2,synthesizer")
    { return firpfbch2_crcf_execute_bench(num_iterations, 4,    2, LIQUID_SYNTHESIZER); }
LIQUID_BENCHMARK(firpfbch2_crcf_s16,   "firpfbch2_crcf synthesizer, M=16",   "multichannel,firpfbch2,synthesizer")
    { return firpfbch2_crcf_execute_bench(num_iterations, 16,   2, LIQUID_SYNTHESIZER); }
LIQUID_BENCHMARK(firpfbch2_crcf_s64,   "firpfbch2_crcf synthesizer, M=64",   "multichannel,firpfbch2,synthesizer")
    { return firpfbch2_crcf_execute_bench(num_iterations, 64,   2, LIQUID_SYNTHESIZER); }
LIQUID_BENCHMARK(firpfbch2_crcf_s256,  "firpfbch2_crcf synthesizer, M=256",  "multichannel,firpfbch2,synthesizer")
    { return firpfbch2_crcf_execute_bench(num_iterations, 256,  2, LIQUID_SYNTHESIZER); }
LIQUID_BENCHMARK(firpfbch2_crcf_s512,  "firpfbch2_crcf synthesizer, M=512",  "multichannel,firpfbch2,synthesizer")
    { return firpfbch2_crcf_execute_bench(num_iterations, 512,  2, LIQUID_SYNTHESIZER); }
LIQUID_BENCHMARK(firpfbch2_crcf_s1024, "firpfbch2_crcf synthesizer, M=1024", "multichannel,firpfbch2,synthesizer")
    { return firpfbch2_crcf_execute_bench(num_iterations, 1024, 2, LIQUID_SYNTHESIZER); }

