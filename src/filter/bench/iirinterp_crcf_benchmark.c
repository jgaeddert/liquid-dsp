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

#include "liquid.benchmark.h"

// Helper function to keep code base small
float iirinterp_crcf_bench(unsigned long int num_iterations,
                          unsigned int _M,
                          unsigned int _order)
{
    // create interpolator from prototype
    liquid_iirdes_filtertype ftype  = LIQUID_IIRDES_BUTTER;
    liquid_iirdes_bandtype   btype  = LIQUID_IIRDES_LOWPASS;
    liquid_iirdes_format     format = LIQUID_IIRDES_SOS;
    float fc =  0.5f / (float)_M;   // filter cut-off frequency
    float f0 =  0.0f;
    float Ap =  0.1f;
    float As = 60.0f;
    iirinterp_crcf q = iirinterp_crcf_create_prototype(_M,ftype,btype,format,_order,fc,f0,Ap,As);

    float complex y[_M];
    // start trials (4 executes of 1 input sample each per iteration; round down)
    unsigned long int n = num_iterations / 4;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    unsigned int i;
    for (i=0; i<n; i++) {
        iirinterp_crcf_execute(q, 1.0f, y);
        iirinterp_crcf_execute(q, 1.0f, y);
        iirinterp_crcf_execute(q, 1.0f, y);
        iirinterp_crcf_execute(q, 1.0f, y);
    }
    float extime = liquid_toc(timer);

    iirinterp_crcf_destroy(q);
    return extime;
}

LIQUID_BENCHMARK(iirinterp_crcf_M2,  "iirinterp_crcf execute, M=2 order=5",  "IIR,interpolator")
    { return iirinterp_crcf_bench(num_iterations, 2,  5); }

LIQUID_BENCHMARK(iirinterp_crcf_M4,  "iirinterp_crcf execute, M=4 order=5",  "IIR,interpolator")
    { return iirinterp_crcf_bench(num_iterations, 4,  5); }

LIQUID_BENCHMARK(iirinterp_crcf_M8,  "iirinterp_crcf execute, M=8 order=5",  "IIR,interpolator")
    { return iirinterp_crcf_bench(num_iterations, 8,  5); }

LIQUID_BENCHMARK(iirinterp_crcf_M16, "iirinterp_crcf execute, M=16 order=5", "IIR,interpolator")
    { return iirinterp_crcf_bench(num_iterations, 16, 5); }

LIQUID_BENCHMARK(iirinterp_crcf_M32, "iirinterp_crcf execute, M=32 order=5", "IIR,interpolator")
    { return iirinterp_crcf_bench(num_iterations, 32, 5); }

