/*
 * Copyright (c) 2007, 2009 Joseph Gaeddert
 * Copyright (c) 2007, 2009 Virginia Polytechnic Institute & State University
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

#ifndef __LIQUID_QMFB_BENCHMARK_H__
#define __LIQUID_QMFB_BENCHMARK_H__

#include <sys/resource.h>
#include "liquid.h"

// Helper function to keep code base small
void qmfb_crcf_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _M,
    unsigned int _h_len)
{
    float h[_h_len];
    unsigned int i;
    for (i=0; i<_h_len; i++)
        h[i] = 1.0f;

    qmfb_crcf q = qmfb_crcf_create(_M,h,_h_len);

    float x[_M], y;
    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        decim_rrrf_execute(q,x,&y,0);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    decim_rrrf_destroy(q);
}

#define QMFB_CRCF_BENCHMARK_API(M,H_LEN)    \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ qmfb_crcf_bench(_start, _finish, _num_iterations, M, H_LEN); }

void benchmark_decim_m2_h8      QMFB_CRCF_BENCHMARK_API(2,8)
void benchmark_decim_m4_h16     QMFB_CRCF_BENCHMARK_API(4,16)
void benchmark_decim_m8_h64     QMFB_CRCF_BENCHMARK_API(8,64)

#endif // __LIQUID_BENCH_BENCHMARK_H__

