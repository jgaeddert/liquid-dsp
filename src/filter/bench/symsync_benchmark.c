/*
 * Copyright (c) 2011 Joseph Gaeddert
 * Copyright (c) 2011 Virginia Polytechnic Institute & State University
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
void symsync_crcf_bench(
    struct rusage *_start,
    struct rusage *_finish,
    unsigned long int *_num_iterations,
    unsigned int _h_len)
{
    unsigned int i;
    unsigned int num_filters=16;
    unsigned int k=2;
    unsigned int m=3;
    float beta=0.3f;

    // create symbol synchronizer
    symsync_crcf q = symsync_crcf_create_rnyquist(LIQUID_RNYQUIST_RRC,
                                                  k, m, beta, num_filters);

    unsigned int num_samples=k;
    unsigned int num_written;
    float complex x[num_samples];
    float complex y[num_samples];

    for (i=0; i<num_samples; i++)
        x[i] = 0.0f;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        symsync_crcf_execute(q,x,num_samples,y,&num_written);
        symsync_crcf_execute(q,x,num_samples,y,&num_written);
        symsync_crcf_execute(q,x,num_samples,y,&num_written);
        symsync_crcf_execute(q,x,num_samples,y,&num_written);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    symsync_crcf_destroy(q);
}

#define SYMSYNC_CRCF_BENCHMARK_API(H_LEN)   \
(   struct rusage *_start,                  \
    struct rusage *_finish,                 \
    unsigned long int *_num_iterations)     \
{ symsync_crcf_bench(_start, _finish, _num_iterations, H_LEN); }

// 
// Interpolators
//
void benchmark_symsync_crcf SYMSYNC_CRCF_BENCHMARK_API(13)   // m=3

