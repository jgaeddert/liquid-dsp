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
void decim_rrrf_bench(struct rusage *_start,
                      struct rusage *_finish,
                      unsigned long int *_num_iterations,
                      unsigned int _M,
                      unsigned int _h_len)
{
    // normalize number of iterations
    *_num_iterations /= _h_len;
    if (*_num_iterations < 1) *_num_iterations = 1;

    float h[_h_len];
    unsigned int i;
    for (i=0; i<_h_len; i++)
        h[i] = 1.0f;

    decim_rrrf q = decim_rrrf_create(_M,h,_h_len);

    float x[_M], y;
    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        decim_rrrf_execute(q,x,&y,0);
        decim_rrrf_execute(q,x,&y,0);
        decim_rrrf_execute(q,x,&y,0);
        decim_rrrf_execute(q,x,&y,0);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    decim_rrrf_destroy(q);
}

#define DECIM_RRRF_BENCHMARK_API(M,H_LEN)   \
(   struct rusage *_start,                  \
    struct rusage *_finish,                 \
    unsigned long int *_num_iterations)     \
{ decim_rrrf_bench(_start, _finish, _num_iterations, M, H_LEN); }

void benchmark_decim_rrrf_m2_h8     DECIM_RRRF_BENCHMARK_API(2, 8)
void benchmark_decim_rrrf_m4_h16    DECIM_RRRF_BENCHMARK_API(4, 16)
void benchmark_decim_rrrf_m8_h32    DECIM_RRRF_BENCHMARK_API(8, 32)
void benchmark_decim_rrrf_m16_h64   DECIM_RRRF_BENCHMARK_API(16,64)
void benchmark_decim_rrrf_m32_h128  DECIM_RRRF_BENCHMARK_API(32,128)

