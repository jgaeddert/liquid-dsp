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
#include <string.h>

LIQUID_BENCHMARK(vco_sincos, "nco_crcf sincos+step (vco)", "nco,sincos,vco")
{
    float s, c;
    nco_crcf p = nco_crcf_create(LIQUID_VCO);
    nco_crcf_set_phase(p, 0.0f);
    nco_crcf_set_frequency(p, 0.1f);
    unsigned int i;
    // start trials
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<num_iterations; i++) {
        nco_crcf_sincos(p, &s, &c);
        nco_crcf_step(p);
    }
    float extime = liquid_toc(timer);
    nco_crcf_destroy(p);
    return extime;
}

LIQUID_BENCHMARK(vco_mix_up, "nco_crcf mix_up (vco)", "nco,mix,vco")
{
    float complex x[16],  y[16];
    memset(x, 0, 16*sizeof(float complex));
    nco_crcf p = nco_crcf_create(LIQUID_VCO);
    nco_crcf_set_phase(p, 0.0f);
    nco_crcf_set_frequency(p, 0.1f);
    unsigned int i;
    // start trials
    unsigned long int n = num_iterations / 16;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        nco_crcf_mix_up(p, x[ 0], &y[ 0]); nco_crcf_step(p);
        nco_crcf_mix_up(p, x[ 1], &y[ 1]); nco_crcf_step(p);
        nco_crcf_mix_up(p, x[ 2], &y[ 2]); nco_crcf_step(p);
        nco_crcf_mix_up(p, x[ 3], &y[ 3]); nco_crcf_step(p);
        nco_crcf_mix_up(p, x[ 4], &y[ 4]); nco_crcf_step(p);
        nco_crcf_mix_up(p, x[ 5], &y[ 5]); nco_crcf_step(p);
        nco_crcf_mix_up(p, x[ 6], &y[ 6]); nco_crcf_step(p);
        nco_crcf_mix_up(p, x[ 7], &y[ 7]); nco_crcf_step(p);
        nco_crcf_mix_up(p, x[ 8], &y[ 8]); nco_crcf_step(p);
        nco_crcf_mix_up(p, x[ 9], &y[ 9]); nco_crcf_step(p);
        nco_crcf_mix_up(p, x[10], &y[10]); nco_crcf_step(p);
        nco_crcf_mix_up(p, x[11], &y[11]); nco_crcf_step(p);
        nco_crcf_mix_up(p, x[12], &y[12]); nco_crcf_step(p);
        nco_crcf_mix_up(p, x[13], &y[13]); nco_crcf_step(p);
        nco_crcf_mix_up(p, x[14], &y[14]); nco_crcf_step(p);
        nco_crcf_mix_up(p, x[15], &y[15]); nco_crcf_step(p);
    }
    float extime = liquid_toc(timer);
    nco_crcf_destroy(p);
    return extime;
}

LIQUID_BENCHMARK(vco_mix_block_up, "nco_crcf mix_block_up (vco)", "nco,mix,vco")
{
    float complex x[16], y[16];
    memset(x, 0, 16*sizeof(float complex));
    nco_crcf p = nco_crcf_create(LIQUID_VCO);
    nco_crcf_set_phase(p, 0.0f);
    nco_crcf_set_frequency(p, 0.1f);
    unsigned int i;
    // start trials (16 mix_block_up per iteration; round down)
    unsigned long int n = num_iterations / 16;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++)
        nco_crcf_mix_block_up(p, x, y, 16);
    float extime = liquid_toc(timer);
    nco_crcf_destroy(p);
    return extime;
}

