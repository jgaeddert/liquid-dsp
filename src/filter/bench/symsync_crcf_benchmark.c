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
void symsync_crcf_bench(struct rusage *     _start,
                        struct rusage *     _finish,
                        unsigned long int * _num_iterations,
                        unsigned int        _k,
                        unsigned int        _m)
{
    unsigned long int i;
    unsigned int npfb = 16;     // number of filters in bank
    unsigned int k    = _k;     // samples/symbol
    unsigned int m    = _m;     // filter delay [symbols]
    float beta        = 0.3f;   // filter excess bandwidth factor

    // create symbol synchronizer
    symsync_crcf q = symsync_crcf_create_rnyquist(LIQUID_FIRFILT_RRC,
                                                  k, m, beta, npfb);

    //
    unsigned int num_samples = 64;
    *_num_iterations /= num_samples;

    unsigned int num_written;
    float complex x[num_samples];
    float complex y[num_samples];

    // generate pseudo-random data
    msequence ms = msequence_create_default(6);
    for (i=0; i<num_samples; i++)
        x[i] = ((float)msequence_generate_symbol(ms, 6) - 31.5) / 24.0f;
    msequence_destroy(ms);

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        symsync_crcf_execute(q, x, num_samples, y, &num_written);
        symsync_crcf_execute(q, x, num_samples, y, &num_written);
        symsync_crcf_execute(q, x, num_samples, y, &num_written);
        symsync_crcf_execute(q, x, num_samples, y, &num_written);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4 * num_samples;

    symsync_crcf_destroy(q);
}

#define SYMSYNC_CRCF_BENCHMARK_API(K,M)     \
(   struct rusage *_start,                  \
    struct rusage *_finish,                 \
    unsigned long int *_num_iterations)     \
{ symsync_crcf_bench(_start, _finish, _num_iterations, K, M); }

// 
// BENCHMARKS
//
void benchmark_symsync_crcf_k2_m2   SYMSYNC_CRCF_BENCHMARK_API(2, 2)
void benchmark_symsync_crcf_k2_m4   SYMSYNC_CRCF_BENCHMARK_API(2, 4)
void benchmark_symsync_crcf_k2_m8   SYMSYNC_CRCF_BENCHMARK_API(2, 8)
void benchmark_symsync_crcf_k2_m16  SYMSYNC_CRCF_BENCHMARK_API(2, 16)

