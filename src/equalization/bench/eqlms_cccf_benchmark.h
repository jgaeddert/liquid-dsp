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

#ifndef __LIQUID_EQLMS_CCCF_BENCHMARK_H__
#define __LIQUID_EQLMS_CCCF_BENCHMARK_H__

#include <sys/resource.h>
#include "liquid.h"

#define EQLMS_CCCF_TRAIN_BENCH_API(N) \
(   struct rusage *_start, \
    struct rusage *_finish, \
    unsigned long int *_num_iterations) \
{ eqlms_cccf_train_bench(_start, _finish, _num_iterations, N); }

// Helper function to keep code base small
void eqlms_cccf_train_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _h_len)
{
    eqlms_cccf eq = eqlms_cccf_create(_h_len);
    
    unsigned long int i;

    float complex y=1.0f + 1.0f*_Complex_I;
    float complex d=1.0f - 1.0f*_Complex_I;
    float complex z;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        eqlms_cccf_execute(eq, y, d, &z);
        eqlms_cccf_execute(eq, y, d, &z);
        eqlms_cccf_execute(eq, y, d, &z);
        eqlms_cccf_execute(eq, y, d, &z);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    eqlms_cccf_destroy(eq);
}

// 
void benchmark_eqlms_cccf_n4    EQLMS_CCCF_TRAIN_BENCH_API(4)
void benchmark_eqlms_cccf_n8    EQLMS_CCCF_TRAIN_BENCH_API(8)
void benchmark_eqlms_cccf_n16   EQLMS_CCCF_TRAIN_BENCH_API(16)
void benchmark_eqlms_cccf_n32   EQLMS_CCCF_TRAIN_BENCH_API(32)
void benchmark_eqlms_cccf_n64   EQLMS_CCCF_TRAIN_BENCH_API(64)

#endif // __LIQUID_EQLMS_CCCF_BENCHMARK_H__

