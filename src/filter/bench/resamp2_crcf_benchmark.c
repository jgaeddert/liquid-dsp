/*
 * Copyright (c) 2007 - 2018 Joseph Gaeddert
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

typedef enum {
    RESAMP2_DECIM,
    RESAMP2_INTERP
} resamp2_type;

// Helper function to keep code base small
float resamp2_crcf_bench(unsigned long int num_iterations,
                        unsigned int _m,
                        resamp2_type _type)
{
    unsigned long int i;

    resamp2_crcf q = resamp2_crcf_create(_m,0.0f,60.0f);

    float complex x[] = {1.0f, -1.0f};
    float complex y[] = {1.0f, -1.0f};

    // work units per loop iteration: decim consumes 2 input samples x4 = 8;
    // interp consumes 1 input sample x4 = 4
    unsigned long int work_per_iter = (_type == RESAMP2_DECIM) ? 8 : 4;
    unsigned long int n = num_iterations / work_per_iter;

    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    if (_type == RESAMP2_DECIM) {
        // run decimator
        for (i=0; i<n; i++) {
            resamp2_crcf_decim_execute(q,x,y);
            resamp2_crcf_decim_execute(q,x,y);
            resamp2_crcf_decim_execute(q,x,y);
            resamp2_crcf_decim_execute(q,x,y);
        }
    } else {
        // run interpolator
        for (i=0; i<n; i++) {
            resamp2_crcf_interp_execute(q,x[0],y);
            resamp2_crcf_interp_execute(q,x[0],y);
            resamp2_crcf_interp_execute(q,x[0],y);
            resamp2_crcf_interp_execute(q,x[0],y);
        }
    }
    float extime = liquid_toc(timer);

    resamp2_crcf_destroy(q);
    return extime;
}

//
// Decimators
//
LIQUID_BENCHMARK(resamp2_crcf_decim_m2,    "resamp2_crcf decim execute, m=2",    "filter,resamp2,halfband,decimator")
    { return resamp2_crcf_bench(num_iterations,   2, RESAMP2_DECIM); }

LIQUID_BENCHMARK(resamp2_crcf_decim_m4,    "resamp2_crcf decim execute, m=4",    "filter,resamp2,halfband,decimator")
    { return resamp2_crcf_bench(num_iterations,   4, RESAMP2_DECIM); }

LIQUID_BENCHMARK(resamp2_crcf_decim_m8,    "resamp2_crcf decim execute, m=8",    "filter,resamp2,halfband,decimator")
    { return resamp2_crcf_bench(num_iterations,   8, RESAMP2_DECIM); }

LIQUID_BENCHMARK(resamp2_crcf_decim_m16,   "resamp2_crcf decim execute, m=16",   "filter,resamp2,halfband,decimator")
    { return resamp2_crcf_bench(num_iterations,  16, RESAMP2_DECIM); }

LIQUID_BENCHMARK(resamp2_crcf_decim_m32,   "resamp2_crcf decim execute, m=32",   "filter,resamp2,halfband,decimator")
    { return resamp2_crcf_bench(num_iterations,  32, RESAMP2_DECIM); }

LIQUID_BENCHMARK(resamp2_crcf_decim_m64,   "resamp2_crcf decim execute, m=64",   "filter,resamp2,halfband,decimator")
    { return resamp2_crcf_bench(num_iterations,  64, RESAMP2_DECIM); }

LIQUID_BENCHMARK(resamp2_crcf_decim_m128,  "resamp2_crcf decim execute, m=128",  "filter,resamp2,halfband,decimator")
    { return resamp2_crcf_bench(num_iterations, 128, RESAMP2_DECIM); }

LIQUID_BENCHMARK(resamp2_crcf_decim_m256,  "resamp2_crcf decim execute, m=256",  "filter,resamp2,halfband,decimator")
    { return resamp2_crcf_bench(num_iterations, 256, RESAMP2_DECIM); }

//
// Interpolators
//
LIQUID_BENCHMARK(resamp2_crcf_interp_m2,   "resamp2_crcf interp execute, m=2",    "filter,resamp2,halfband,interpolator")
    { return resamp2_crcf_bench(num_iterations,   2, RESAMP2_INTERP); }

LIQUID_BENCHMARK(resamp2_crcf_interp_m4,   "resamp2_crcf interp execute, m=4",    "filter,resamp2,halfband,interpolator")
    { return resamp2_crcf_bench(num_iterations,   4, RESAMP2_INTERP); }

LIQUID_BENCHMARK(resamp2_crcf_interp_m8,   "resamp2_crcf interp execute, m=8",    "filter,resamp2,halfband,interpolator")
    { return resamp2_crcf_bench(num_iterations,   8, RESAMP2_INTERP); }

LIQUID_BENCHMARK(resamp2_crcf_interp_m16,  "resamp2_crcf interp execute, m=16",   "filter,resamp2,halfband,interpolator")
    { return resamp2_crcf_bench(num_iterations,  16, RESAMP2_INTERP); }

LIQUID_BENCHMARK(resamp2_crcf_interp_m32,  "resamp2_crcf interp execute, m=32",   "filter,resamp2,halfband,interpolator")
    { return resamp2_crcf_bench(num_iterations,  32, RESAMP2_INTERP); }

LIQUID_BENCHMARK(resamp2_crcf_interp_m64,  "resamp2_crcf interp execute, m=64",   "filter,resamp2,halfband,interpolator")
    { return resamp2_crcf_bench(num_iterations,  64, RESAMP2_INTERP); }

LIQUID_BENCHMARK(resamp2_crcf_interp_m128, "resamp2_crcf interp execute, m=128",  "filter,resamp2,halfband,interpolator")
    { return resamp2_crcf_bench(num_iterations, 128, RESAMP2_INTERP); }

LIQUID_BENCHMARK(resamp2_crcf_interp_m256, "resamp2_crcf interp execute, m=256",  "filter,resamp2,halfband,interpolator")
    { return resamp2_crcf_bench(num_iterations, 256, RESAMP2_INTERP); }

