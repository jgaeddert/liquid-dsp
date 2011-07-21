/*
 * Copyright (c) 2007, 2009, 2011 Joseph Gaeddert
 * Copyright (c) 2007, 2009, 2011 Virginia Polytechnic Institute & State University
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
void iirfilt_crcf_bench(struct rusage *_start,
                        struct rusage *_finish,
                        unsigned long int *_num_iterations,
                        unsigned int _n,
                        unsigned int _type)
{
    unsigned int i;

    // scale number of iterations (trials)
    if (_type == 0) {
        // cycles/trial ~ 128 + 15.3*_n
        *_num_iterations *= 1000;
        *_num_iterations /= (unsigned int)(128 + 15.3*_n);
    } else {
        // cycles/trial ~ 93 + 53.3*_n
        *_num_iterations *= 800;
        *_num_iterations /= (unsigned int)(93 + 53.3*_n);
    }

    // create filter object
    iirfilt_crcf q;
    if (_type == 0) {
        // normal transfer function
        float b[_n], a[_n];
        for (i=0; i<_n; i++) {
            b[i] = 1.0f;
            a[i] = i==0 ? 1.0f : 0.0f;
        }
        q = iirfilt_crcf_create(b,_n,a,_n);
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

        q = iirfilt_crcf_create_sos(B,A,L+r);
    }

    // initialize input/output
    float complex x[4];
    float complex y[4];
    for (i=0; i<4; i++)
        x[i] = randnf() + _Complex_I*randnf();

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        iirfilt_crcf_execute(q, x[0], &y[0]);
        iirfilt_crcf_execute(q, x[1], &y[1]);
        iirfilt_crcf_execute(q, x[2], &y[2]);
        iirfilt_crcf_execute(q, x[3], &y[3]);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    iirfilt_crcf_destroy(q);

}

#define IIRFILT_CRCF_BENCHMARK_API(N,T)     \
(   struct rusage *_start,                  \
    struct rusage *_finish,                 \
    unsigned long int *_num_iterations)     \
{ iirfilt_crcf_bench(_start, _finish, _num_iterations, N, T); }

void benchmark_iirfilt_crcf_4        IIRFILT_CRCF_BENCHMARK_API(4,    0)
void benchmark_iirfilt_crcf_8        IIRFILT_CRCF_BENCHMARK_API(8,    0)
void benchmark_iirfilt_crcf_16       IIRFILT_CRCF_BENCHMARK_API(16,   0)
void benchmark_iirfilt_crcf_32       IIRFILT_CRCF_BENCHMARK_API(32,   0)
void benchmark_iirfilt_crcf_64       IIRFILT_CRCF_BENCHMARK_API(64,   0)

void benchmark_iirfilt_crcf_sos_4    IIRFILT_CRCF_BENCHMARK_API(4,    1)
void benchmark_iirfilt_crcf_sos_8    IIRFILT_CRCF_BENCHMARK_API(8,    1)
void benchmark_iirfilt_crcf_sos_16   IIRFILT_CRCF_BENCHMARK_API(16,   1)
void benchmark_iirfilt_crcf_sos_32   IIRFILT_CRCF_BENCHMARK_API(32,   1)
void benchmark_iirfilt_crcf_sos_64   IIRFILT_CRCF_BENCHMARK_API(64,   1)

