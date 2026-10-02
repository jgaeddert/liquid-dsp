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
#include <math.h>

// Helper function to keep code base small
float fskmod_bench(unsigned long int _num_iterations,
                   unsigned int      _m,
                   unsigned int      _k,
                   float             _bandwidth)
{
    // initialize modulator
    fskmod mod = fskmod_create(_m,_k,_bandwidth);
    unsigned int M = 1 << _m;   // constellation size
    float complex buf[_k];      // transmit buffer
    
    unsigned long int i;
    // start trials (10 modulates per iteration; round down)
    unsigned long int n = _num_iterations / 10;
    if (n < 1) n = 1;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        fskmod_modulate(mod, (i+ 0) % M, buf);
        fskmod_modulate(mod, (i+ 1) % M, buf);
        fskmod_modulate(mod, (i+ 2) % M, buf);
        fskmod_modulate(mod, (i+ 3) % M, buf);
        fskmod_modulate(mod, (i+ 4) % M, buf);
        fskmod_modulate(mod, (i+ 5) % M, buf);
        fskmod_modulate(mod, (i+ 6) % M, buf);
        fskmod_modulate(mod, (i+ 7) % M, buf);
        fskmod_modulate(mod, (i+ 8) % M, buf);
        fskmod_modulate(mod, (i+ 9) % M, buf);
    }
    float extime = liquid_toc(timer);
    fskmod_destroy(mod);
    return extime;
}

// BENCHMARKS: basic properties: M=2^m, k = 2*M, bandwidth = 0.25
LIQUID_BENCHMARK(fskmod_norm_M2,    "fskmod modulate, M=2 k=4 bw=0.25",          "modem,fsk")
    { return fskmod_bench(num_iterations, 1,    4, 0.25f    ); }
LIQUID_BENCHMARK(fskmod_norm_M4,    "fskmod modulate, M=4 k=8 bw=0.25",          "modem,fsk")
    { return fskmod_bench(num_iterations, 2,    8, 0.25f    ); }
LIQUID_BENCHMARK(fskmod_norm_M8,    "fskmod modulate, M=8 k=16 bw=0.25",         "modem,fsk")
    { return fskmod_bench(num_iterations, 3,   16, 0.25f    ); }
LIQUID_BENCHMARK(fskmod_norm_M16,   "fskmod modulate, M=16 k=32 bw=0.25",        "modem,fsk")
    { return fskmod_bench(num_iterations, 4,   32, 0.25f    ); }
LIQUID_BENCHMARK(fskmod_norm_M32,   "fskmod modulate, M=32 k=64 bw=0.25",        "modem,fsk")
    { return fskmod_bench(num_iterations, 5,   64, 0.25f    ); }
LIQUID_BENCHMARK(fskmod_norm_M64,   "fskmod modulate, M=64 k=128 bw=0.25",       "modem,fsk")
    { return fskmod_bench(num_iterations, 6,  128, 0.25f    ); }
LIQUID_BENCHMARK(fskmod_norm_M128,  "fskmod modulate, M=128 k=256 bw=0.25",      "modem,fsk")
    { return fskmod_bench(num_iterations, 7,  256, 0.25f    ); }
LIQUID_BENCHMARK(fskmod_norm_M256,  "fskmod modulate, M=256 k=512 bw=0.25",      "modem,fsk")
    { return fskmod_bench(num_iterations, 8,  512, 0.25f    ); }
LIQUID_BENCHMARK(fskmod_norm_M512,  "fskmod modulate, M=512 k=1024 bw=0.25",     "modem,fsk")
    { return fskmod_bench(num_iterations, 9, 1024, 0.25f    ); }
LIQUID_BENCHMARK(fskmod_norm_M1024, "fskmod modulate, M=1024 k=2048 bw=0.25",    "modem,fsk")
    { return fskmod_bench(num_iterations, 10, 2048, 0.25f    ); }

// BENCHMARKS: obscure properties: M=2^m, k not relative to M, bandwidth basically irrational
LIQUID_BENCHMARK(fskmod_misc_M2,    "fskmod modulate, M=2 k=5 bw=0.372",      "modem,fsk")
    { return fskmod_bench(num_iterations, 1,    5, 0.3721451); }
LIQUID_BENCHMARK(fskmod_misc_M4,    "fskmod modulate, M=4 k=10 bw=0.372",     "modem,fsk")
    { return fskmod_bench(num_iterations, 2,   10, 0.3721451); }
LIQUID_BENCHMARK(fskmod_misc_M8,    "fskmod modulate, M=8 k=20 bw=0.372",      "modem,fsk")
    { return fskmod_bench(num_iterations, 3,   20, 0.3721451); }
LIQUID_BENCHMARK(fskmod_misc_M16,   "fskmod modulate, M=16 k=30 bw=0.372",     "modem,fsk")
    { return fskmod_bench(num_iterations, 4,   30, 0.3721451); }
LIQUID_BENCHMARK(fskmod_misc_M32,   "fskmod modulate, M=32 k=60 bw=0.372",     "modem,fsk")
    { return fskmod_bench(num_iterations, 5,   60, 0.3721451); }
LIQUID_BENCHMARK(fskmod_misc_M64,   "fskmod modulate, M=64 k=100 bw=0.372",    "modem,fsk")
    { return fskmod_bench(num_iterations, 6,  100, 0.3721451); }
LIQUID_BENCHMARK(fskmod_misc_M128,  "fskmod modulate, M=128 k=200 bw=0.372",   "modem,fsk")
    { return fskmod_bench(num_iterations, 7,  200, 0.3721451); }
LIQUID_BENCHMARK(fskmod_misc_M256,  "fskmod modulate, M=256 k=500 bw=0.372",   "modem,fsk")
    { return fskmod_bench(num_iterations, 8,  500, 0.3721451); }
LIQUID_BENCHMARK(fskmod_misc_M512,  "fskmod modulate, M=512 k=1000 bw=0.372",  "modem,fsk")
    { return fskmod_bench(num_iterations, 9, 1000, 0.3721451); }
LIQUID_BENCHMARK(fskmod_misc_M1024, "fskmod modulate, M=1024 k=2000 bw=0.372", "modem,fsk")
    { return fskmod_bench(num_iterations, 10, 2000, 0.3721451); }

