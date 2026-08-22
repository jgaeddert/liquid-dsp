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
#include <assert.h>

// Helper function to keep code base small
float ofdmframesync_acquire_bench(unsigned long int _num_iterations,
                                  unsigned int      _num_subcarriers,
                                  unsigned int      _cp_len)
{
    // options
    unsigned int M         = _num_subcarriers;
    unsigned int cp_len    = _cp_len;
    unsigned int taper_len = 0;

    // derived values
    unsigned int num_samples = 3*(M + cp_len);

    // create synthesizer/analyzer objects
    ofdmframegen fg = ofdmframegen_create(M, cp_len, taper_len, NULL);
    //ofdmframegen_print(fg);
    ofdmframesync fs = ofdmframesync_create(M,cp_len,taper_len,NULL,NULL,NULL);
    unsigned int i;
    float complex y[num_samples];   // frame samples
    // assemble full frame
    unsigned int n=0;
    // write first S0 symbol
    ofdmframegen_write_S0a(fg, &y[n]);
    n += M + cp_len;
    // write second S0 symbol
    ofdmframegen_write_S0b(fg, &y[n]);
    n += M + cp_len;
    // write S1 symbol
    ofdmframegen_write_S1( fg, &y[n]);
    n += M + cp_len;
    assert(n == num_samples);
    // add noise
    for (i=0; i<num_samples; i++)
        y[i] += 0.02f*randnf()*cexpf(_Complex_I*2*M_PI*randf());
    // start trials (1 execute of num_samples samples per iteration; round down)
    unsigned long int iters = _num_iterations / num_samples;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<iters; i++) {
        ofdmframesync_execute(fs,y,num_samples);
        ofdmframesync_reset(fs);
    }
    float extime = liquid_toc(timer);
    // destroy objects
    ofdmframegen_destroy(fg);
    ofdmframesync_destroy(fs);
    return extime;
}

LIQUID_BENCHMARK(ofdmframesync_acquire_n64,  "ofdmframesync acquire, M=64 cp_len=8",  "framing,ofdmframe,acquire")
    { return ofdmframesync_acquire_bench(num_iterations, 64, 8); }
LIQUID_BENCHMARK(ofdmframesync_acquire_n128, "ofdmframesync acquire, M=128 cp_len=16", "framing,ofdmframe,acquire")
    { return ofdmframesync_acquire_bench(num_iterations, 128, 16); }
LIQUID_BENCHMARK(ofdmframesync_acquire_n256, "ofdmframesync acquire, M=256 cp_len=32", "framing,ofdmframe,acquire")
    { return ofdmframesync_acquire_bench(num_iterations, 256, 32); }
LIQUID_BENCHMARK(ofdmframesync_acquire_n512, "ofdmframesync acquire, M=512 cp_len=64", "framing,ofdmframe,acquire")
    { return ofdmframesync_acquire_bench(num_iterations, 512, 64); }

