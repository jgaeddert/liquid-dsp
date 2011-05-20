/*
 * Copyright (c) 2007, 2008, 2009, 2010 Joseph Gaeddert
 * Copyright (c) 2007, 2008, 2009, 2010 Virginia Polytechnic
 *                                      Institute & State University
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
void interp_rrrf_bench(struct rusage *_start,
                       struct rusage *_finish,
                       unsigned long int *_num_iterations,
                       unsigned int _M,
                       unsigned int _h_len)
{
    // normalize number of iterations
    *_num_iterations *= 80;
    *_num_iterations /= _h_len;
    if (*_num_iterations < 1) *_num_iterations = 1;

    float h[_h_len];
    unsigned int i;
    for (i=0; i<_h_len; i++)
        h[i] = 1.0f;

    interp_rrrf q = interp_rrrf_create(_M,h,_h_len);

    float y[_M];
    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        interp_rrrf_execute(q,1.0f,y);
        interp_rrrf_execute(q,1.0f,y);
        interp_rrrf_execute(q,1.0f,y);
        interp_rrrf_execute(q,1.0f,y);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    interp_rrrf_destroy(q);
}

#define INTERP_RRRF_BENCHMARK_API(M,H_LEN)  \
(   struct rusage *_start,                  \
    struct rusage *_finish,                 \
    unsigned long int *_num_iterations)     \
{ interp_rrrf_bench(_start, _finish, _num_iterations, M, H_LEN); }

void benchmark_interp_rrrf_m2_h8    INTERP_RRRF_BENCHMARK_API(2, 8)
void benchmark_interp_rrrf_m4_h16   INTERP_RRRF_BENCHMARK_API(4, 16)
void benchmark_interp_rrrf_m8_h32   INTERP_RRRF_BENCHMARK_API(8, 32)
void benchmark_interp_rrrf_m16_h64  INTERP_RRRF_BENCHMARK_API(16,64)
void benchmark_interp_rrrf_m32_h128 INTERP_RRRF_BENCHMARK_API(32,128)

