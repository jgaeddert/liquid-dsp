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

#define WINDOW_READ_BENCH_API(N)        \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ window_read_bench(_start, _finish, _num_iterations, N); }

// Helper function to keep code base small
void window_read_bench(struct rusage *_start,
                       struct rusage *_finish,
                       unsigned long int *_num_iterations,
                       unsigned int _n)
{
    // normalize number of iterations
    if (*_num_iterations < 1) *_num_iterations = 1;

    // initialize port
    windowcf w = windowcf_create(_n);

    unsigned long int i;

    float complex * r;

    // start trials:
    //   write to buffer, read from buffer
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        windowcf_push(w,1.0f);  windowcf_read(w, &r);
        windowcf_push(w,1.0f);  windowcf_read(w, &r);
        windowcf_push(w,1.0f);  windowcf_read(w, &r);
        windowcf_push(w,1.0f);  windowcf_read(w, &r);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    windowcf_destroy(w);
}

// 
void benchmark_windowcf_read_n16    WINDOW_READ_BENCH_API(16)
void benchmark_windowcf_read_n32    WINDOW_READ_BENCH_API(32)
void benchmark_windowcf_read_n64    WINDOW_READ_BENCH_API(64)
void benchmark_windowcf_read_n128   WINDOW_READ_BENCH_API(128)
void benchmark_windowcf_read_n256   WINDOW_READ_BENCH_API(256)

