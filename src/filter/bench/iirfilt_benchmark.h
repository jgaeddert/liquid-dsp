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

#ifndef __LIQUID_IIRFILT_RRRF_BENCHMARK_H__
#define __LIQUID_IIRFILT_RRRF_BENCHMARK_H__

#include <sys/resource.h>
#include "liquid.h"

// Helper function to keep code base small
void iirfilt_rrrf_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _n,
    unsigned int _type)
{
    unsigned int i;

    // scale number of iterations (trials)
    *_num_iterations = (*_num_iterations * 16) / _n;

    // create filter object
    iirfilt_rrrf f;
    if (_type == 0) {
        // normal transfer function
        float b[_n], a[_n];
        for (i=0; i<_n; i++) {
            b[i] = 1.0f;
            a[i] = i==0 ? 1.0f : 0.0f;
        }
        f = iirfilt_rrrf_create(b,_n,a,_n);
    } else {
        // second-order sections
        unsigned int r = _n % 2;
        unsigned int L = (_n-r)/2;
        float B[3*(L+r)];
        float A[3*(L+r)];
        for (i=0; i<L+r; i++) {
            B[3*i+0] = 1.0f;
            B[3*i+1] = 1.0f;
            B[3*i+2] = 1.0f;

            A[3*i+0] = 1.0f;
            A[3*i+1] = 0.0f;
            A[3*i+2] = 0.0f;
        }

        f = iirfilt_rrrf_create_sos(B,A,L+r);
    }

    // start trials
    float y;
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        iirfilt_rrrf_execute(f,1.0f,&y);
        iirfilt_rrrf_execute(f,1.0f,&y);
        iirfilt_rrrf_execute(f,1.0f,&y);
        iirfilt_rrrf_execute(f,1.0f,&y);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    iirfilt_rrrf_destroy(f);

}

#define IIRFILT_RRRF_BENCHMARK_API(N,T)  \
(   struct rusage *_start,                  \
    struct rusage *_finish,                 \
    unsigned long int *_num_iterations)     \
{ iirfilt_rrrf_bench(_start, _finish, _num_iterations, N, T); }

void benchmark_iirfilt_rrrf_4        IIRFILT_RRRF_BENCHMARK_API(4,    0)
void benchmark_iirfilt_rrrf_8        IIRFILT_RRRF_BENCHMARK_API(8,    0)
void benchmark_iirfilt_rrrf_16       IIRFILT_RRRF_BENCHMARK_API(16,   0)
void benchmark_iirfilt_rrrf_32       IIRFILT_RRRF_BENCHMARK_API(32,   0)
void benchmark_iirfilt_rrrf_64       IIRFILT_RRRF_BENCHMARK_API(64,   0)

void benchmark_iirfilt_rrrf_sos_4    IIRFILT_RRRF_BENCHMARK_API(4,    1)
void benchmark_iirfilt_rrrf_sos_8    IIRFILT_RRRF_BENCHMARK_API(8,    1)
void benchmark_iirfilt_rrrf_sos_16   IIRFILT_RRRF_BENCHMARK_API(16,   1)
void benchmark_iirfilt_rrrf_sos_32   IIRFILT_RRRF_BENCHMARK_API(32,   1)
void benchmark_iirfilt_rrrf_sos_64   IIRFILT_RRRF_BENCHMARK_API(64,   1)

#endif // __LIQUID_IIRFILT_RRRF_BENCHMARK_H__

