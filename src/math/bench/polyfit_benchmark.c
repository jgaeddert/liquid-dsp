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
void polyf_fit_bench(struct rusage *_start,
                     struct rusage *_finish,
                     unsigned long int *_num_iterations,
                     unsigned int _Q,
                     unsigned int _N)
{
    // normalize number of iterations
    // time ~ 0.2953 + 0.03381 * _N
    *_num_iterations /= 0.2953 + 0.03381 * _N;
    if (*_num_iterations < 1) *_num_iterations = 1;

    float p[_Q+1];

    float x[_N];
    float y[_N];
    unsigned int i;
    for (i=0; i<_N; i++) {
        x[i] = randnf();
        y[i] = randnf();
    }
    
    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        polyf_fit(x,y,_N, p,_Q+1);
        polyf_fit(x,y,_N, p,_Q+1);
        polyf_fit(x,y,_N, p,_Q+1);
        polyf_fit(x,y,_N, p,_Q+1);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;
}

#define POLYF_FIT_BENCHMARK_API(Q,N)    \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ polyf_fit_bench(_start, _finish, _num_iterations, Q, N); }

void benchmark_polyf_fit_q3_n8      POLYF_FIT_BENCHMARK_API(3, 8)
void benchmark_polyf_fit_q3_n16     POLYF_FIT_BENCHMARK_API(3, 16)
void benchmark_polyf_fit_q3_n32     POLYF_FIT_BENCHMARK_API(3, 32)
void benchmark_polyf_fit_q3_n64     POLYF_FIT_BENCHMARK_API(3, 64)
void benchmark_polyf_fit_q3_n128    POLYF_FIT_BENCHMARK_API(3, 128)

