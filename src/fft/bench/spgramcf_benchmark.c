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

// benchmark spgram objects
#include "liquid.benchmark.h"
#include <stdlib.h>

// Helper function to keep code base small
float spgramcf_runbench(unsigned long int num_iterations, unsigned int _nfft)
{
    // create object
    spgramcf q = spgramcf_create_default(_nfft);


    // initialize buffer with random values
    unsigned long int i;
    unsigned int buf_len = 2400;
    float complex * buf = (float complex*) malloc(buf_len*sizeof(float complex));
    for (i=0; i<buf_len; i++)
        buf[i] = randnf() + randnf()*_Complex_I;

    // buffer for holding PSD output
    float psd[_nfft];

    // start trials (1 write+get_psd of buf_len samples each per iteration; round down)
    unsigned long int n = num_iterations / buf_len;
    liquid_timer timer = liquid_timer_create(LIQUID_TIMER_RUSAGE);
    for (i=0; i<n; i++) {
        // process input
        spgramcf_write(q, buf, buf_len);
        // get spectrum and feed back to input
        spgramcf_get_psd(q, psd);
        buf[0] = psd[0];
    }
    float extime = liquid_toc(timer);

    free(buf);
    spgramcf_destroy(q);
    return extime;
}

// run several configurations
LIQUID_BENCHMARK(spgramcf_1200,   "spgramcf execute, nfft=1200",   "fft,spgram")
    { return spgramcf_runbench(num_iterations, 1200); }
LIQUID_BENCHMARK(spgramcf_9600,   "spgramcf execute, nfft=9600",   "fft,spgram")
    { return spgramcf_runbench(num_iterations, 9600); }
LIQUID_BENCHMARK(spgramcf_76800,  "spgramcf execute, nfft=76800",  "fft,spgram")
    { return spgramcf_runbench(num_iterations, 76800); }
LIQUID_BENCHMARK(spgramcf_614400, "spgramcf execute, nfft=614400", "fft,spgram")
    { return spgramcf_runbench(num_iterations, 614400); }

