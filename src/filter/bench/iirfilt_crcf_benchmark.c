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
float iirfilt_crcf_bench(unsigned long int num_iterations,
                        unsigned int        _order,
                        unsigned int        _format)
{
    unsigned int i;

    // create filter object from prototype
    float fc    =  0.2f;    // filter cut-off frequency
    float f0    =  0.0f;    // filter center frequency (band-pass, band-stop)
    float Ap    =  0.1f;    // filter pass-band ripple
    float As    = 60.0f;    // filter stop-band attenuation
    iirfilt_crcf q = iirfilt_crcf_create_prototype(LIQUID_IIRDES_BUTTER,
                                                   LIQUID_IIRDES_LOWPASS,
                                                   _format,
                                                   _order,
                                                   fc, f0, Ap, As);

    // initialize input/output
    float complex x[4];
    float complex y[4];
    for (i=0; i<4; i++)
        x[i] = randnf() + _Complex_I*randnf();

    // start trials (4 single-sample executes per iteration; round down)
    unsigned long int n = num_iterations / 4;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        iirfilt_crcf_execute(q, x[0], &y[0]);
        iirfilt_crcf_execute(q, x[1], &y[1]);
        iirfilt_crcf_execute(q, x[2], &y[2]);
        iirfilt_crcf_execute(q, x[3], &y[3]);
    }
    float extime = liquid_toc(timer);

    // destroy filter object
    iirfilt_crcf_destroy(q);
    return extime;
}

// benchmark regular transfer function form
LIQUID_BENCHMARK(iirfilt_crcf_4,      "iirfilt_crcf execute, order=4 (tf)",   "filter,iirfilt,filter")
    { return iirfilt_crcf_bench(num_iterations, 4,  LIQUID_IIRDES_TF); }

LIQUID_BENCHMARK(iirfilt_crcf_8,      "iirfilt_crcf execute, order=8 (tf)",   "filter,iirfilt,filter")
    { return iirfilt_crcf_bench(num_iterations, 8,  LIQUID_IIRDES_TF); }

LIQUID_BENCHMARK(iirfilt_crcf_16,     "iirfilt_crcf execute, order=16 (tf)",  "filter,iirfilt,filter")
    { return iirfilt_crcf_bench(num_iterations, 16, LIQUID_IIRDES_TF); }

LIQUID_BENCHMARK(iirfilt_crcf_32,     "iirfilt_crcf execute, order=32 (tf)",  "filter,iirfilt,filter")
    { return iirfilt_crcf_bench(num_iterations, 32, LIQUID_IIRDES_TF); }

LIQUID_BENCHMARK(iirfilt_crcf_64,     "iirfilt_crcf execute, order=64 (tf)",  "filter,iirfilt,filter")
    { return iirfilt_crcf_bench(num_iterations, 64, LIQUID_IIRDES_TF); }

// benchmark second-order sections form
LIQUID_BENCHMARK(iirfilt_crcf_sos_4,  "iirfilt_crcf execute, order=4 (sos)",  "filter,iirfilt,filter,sos")
    { return iirfilt_crcf_bench(num_iterations, 4,  LIQUID_IIRDES_SOS); }

LIQUID_BENCHMARK(iirfilt_crcf_sos_8,  "iirfilt_crcf execute, order=8 (sos)",  "filter,iirfilt,filter,sos")
    { return iirfilt_crcf_bench(num_iterations, 8,  LIQUID_IIRDES_SOS); }

LIQUID_BENCHMARK(iirfilt_crcf_sos_16, "iirfilt_crcf execute, order=16 (sos)", "filter,iirfilt,filter,sos")
    { return iirfilt_crcf_bench(num_iterations, 16, LIQUID_IIRDES_SOS); }

LIQUID_BENCHMARK(iirfilt_crcf_sos_32, "iirfilt_crcf execute, order=32 (sos)", "filter,iirfilt,filter,sos")
    { return iirfilt_crcf_bench(num_iterations, 32, LIQUID_IIRDES_SOS); }

LIQUID_BENCHMARK(iirfilt_crcf_sos_64, "iirfilt_crcf execute, order=64 (sos)", "filter,iirfilt,filter,sos")
    { return iirfilt_crcf_bench(num_iterations, 64, LIQUID_IIRDES_SOS); }

// benchmark DC-blocking filter
float iirfilt_crcf_dcblock_bench(unsigned long int num_iterations)
{
    unsigned long int i;

    // create filter object
    iirfilt_crcf q = iirfilt_crcf_create_dc_blocker(0.1f);

    // initialize input/output
    float complex x[4];
    for (i=0; i<4; i++)
        x[i] = randnf() + _Complex_I*randnf();

    // start trials (4 single-sample executes per iteration; round down)
    unsigned long int n = num_iterations / 4;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        iirfilt_crcf_execute(q, x[0], &x[0]);
        iirfilt_crcf_execute(q, x[1], &x[1]);
        iirfilt_crcf_execute(q, x[2], &x[2]);
        iirfilt_crcf_execute(q, x[3], &x[3]);
    }
    float extime = liquid_toc(timer);

    // destroy filter object
    iirfilt_crcf_destroy(q);
    return extime;
}

LIQUID_BENCHMARK(iirfilt_crcf_dcblock, "iirfilt_crcf dc-blocker execute", "filter,iirfilt,filter,dcblock")
    { return iirfilt_crcf_dcblock_bench(num_iterations); }

