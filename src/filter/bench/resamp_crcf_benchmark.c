/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
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

