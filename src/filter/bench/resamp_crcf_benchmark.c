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

#include <sys/resource.h>
#include "liquid.h"

// Helper function to keep code base small
void resamp_crcf_bench(struct rusage *     _start,
                       struct rusage *     _finish,
                       unsigned long int * _num_iterations,
                       unsigned int        _m)
{
    unsigned long int i;
    float r = 1.03f;        // resampling rate
    float bw = 0.35f;       // filter bandwidth
    float As = 60.0f;       // stop-band attenuation [dB]
    unsigned int npfb = 32; // number of polyphase filters
    unsigned int m = _m;    // filter semi-length

    resamp_crcf q = resamp_crcf_create(r,m,bw,As,npfb);

    float complex y[4];

    unsigned int num_written;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        resamp_crcf_execute(q, 1.0f, y, &num_written);
        resamp_crcf_execute(q, 1.1f, y, &num_written);
        resamp_crcf_execute(q, 0.9f, y, &num_written);
        resamp_crcf_execute(q, 1.0f, y, &num_written);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    resamp_crcf_destroy(q);
}

#define RESAMP_CRCF_BENCHMARK_API(M)    \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ resamp_crcf_bench(_start, _finish, _num_iterations, M); }

//
// Resampler benchmark prototypes
//
void benchmark_resamp_crcf_m4    RESAMP_CRCF_BENCHMARK_API(4)
void benchmark_resamp_crcf_m8    RESAMP_CRCF_BENCHMARK_API(8)
void benchmark_resamp_crcf_m16   RESAMP_CRCF_BENCHMARK_API(16)
void benchmark_resamp_crcf_m32   RESAMP_CRCF_BENCHMARK_API(32)
void benchmark_resamp_crcf_m64   RESAMP_CRCF_BENCHMARK_API(64)
void benchmark_resamp_crcf_m128  RESAMP_CRCF_BENCHMARK_API(128)

