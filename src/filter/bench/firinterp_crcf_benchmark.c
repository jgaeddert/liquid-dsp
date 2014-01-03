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
void firinterp_crcf_bench(struct rusage *_start,
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

    firinterp_crcf q = firinterp_crcf_create(_M,h,_h_len);

    float complex y[_M];
    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        firinterp_crcf_execute(q,1.0f,y);
        firinterp_crcf_execute(q,1.0f,y);
        firinterp_crcf_execute(q,1.0f,y);
        firinterp_crcf_execute(q,1.0f,y);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    firinterp_crcf_destroy(q);
}

#define FIRINTERP_CRCF_BENCHMARK_API(M,H_LEN)  \
(   struct rusage *_start,                  \
    struct rusage *_finish,                 \
    unsigned long int *_num_iterations)     \
{ firinterp_crcf_bench(_start, _finish, _num_iterations, M, H_LEN); }

void benchmark_firinterp_crcf_m2_h8    FIRINTERP_CRCF_BENCHMARK_API(2, 8)
void benchmark_firinterp_crcf_m4_h16   FIRINTERP_CRCF_BENCHMARK_API(4, 16)
void benchmark_firinterp_crcf_m8_h32   FIRINTERP_CRCF_BENCHMARK_API(8, 32)
void benchmark_firinterp_crcf_m16_h64  FIRINTERP_CRCF_BENCHMARK_API(16,64)
void benchmark_firinterp_crcf_m32_h128 FIRINTERP_CRCF_BENCHMARK_API(32,128)

