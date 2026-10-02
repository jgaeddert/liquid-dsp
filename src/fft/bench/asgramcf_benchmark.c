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

// benchmark asgram objects
#include "liquid.benchmark.h"
#include <stdlib.h>

// Helper function to keep code base small
float asgramcf_runbench(unsigned long int num_iterations,
                       unsigned int        _nfft,
                       int                 _autoscale)
{
    // create object
    asgramcf q = asgramcf_create(_nfft);
    if (_autoscale)
        asgramcf_autoscale_enable(q);


    // initialize buffer with random values
    unsigned long int i;
    unsigned int buf_len = 2400;
    float complex * buf = (float complex*) malloc(buf_len*sizeof(float complex));
    for (i=0; i<buf_len; i++)
        buf[i] = randnf() + randnf()*_Complex_I;

    // buffer for holding ASCII PSD output
    char psd[_nfft];
    float peakval, peakfreq;

    // start trials (1 write+execute of buf_len samples each per iteration; round down)
    unsigned long int n = num_iterations / buf_len;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        // process input
        asgramcf_write(q, buf, buf_len);

        // get spectrum and feed back to input
        asgramcf_execute(q, psd, &peakval, &peakfreq);
        buf[0] = psd[0];
    }
    float extime = liquid_toc(timer);


    free(buf);
    asgramcf_destroy(q);
    return extime;
}

// run several configurations (autoscale disabled)
LIQUID_BENCHMARK(asgramcf_64,  "asgramcf execute, nfft=64",  "fft,asgram")
    { return asgramcf_runbench(num_iterations, 64, 0); }
LIQUID_BENCHMARK(asgramcf_80,  "asgramcf execute, nfft=80",  "fft,asgram")
    { return asgramcf_runbench(num_iterations, 80, 0); }
LIQUID_BENCHMARK(asgramcf_96,  "asgramcf execute, nfft=96",  "fft,asgram")
    { return asgramcf_runbench(num_iterations, 96, 0); }
LIQUID_BENCHMARK(asgramcf_120, "asgramcf execute, nfft=120", "fft,asgram")
    { return asgramcf_runbench(num_iterations, 120, 0); }

// run several configurations (autoscale enabled)
LIQUID_BENCHMARK(asgramcf_64_autoscale,  "asgramcf execute, nfft=64 (autoscale)",  "fft,asgram,autoscale")
    { return asgramcf_runbench(num_iterations, 64, 1); }
LIQUID_BENCHMARK(asgramcf_80_autoscale,  "asgramcf execute, nfft=80 (autoscale)",  "fft,asgram,autoscale")
    { return asgramcf_runbench(num_iterations, 80, 1); }
LIQUID_BENCHMARK(asgramcf_96_autoscale,  "asgramcf execute, nfft=96 (autoscale)",  "fft,asgram,autoscale")
    { return asgramcf_runbench(num_iterations, 96, 1); }
LIQUID_BENCHMARK(asgramcf_120_autoscale, "asgramcf execute, nfft=120 (autoscale)", "fft,asgram,autoscale")
    { return asgramcf_runbench(num_iterations, 120, 1); }

